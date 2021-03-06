#include "FileTransfer.h"

FileTransfer::FileTransfer(string ipAddress,
                           boost::filesystem::path sourceString)
    : ip(ipAddress),
      source(sourceString),
      overallSize(0),
      transferStart(std::chrono::system_clock::now()) {
  if (is_directory(source)) {
    /*Prepare the iterators to explore the folder.*/
    boost::filesystem::recursive_directory_iterator rdi(source.generic_path());
    this->rdi = rdi;
    boost::filesystem::recursive_directory_iterator end_rdi;
    this->end_rdi = end_rdi;
  }
  success.store(true);
  overallSize = 1;
  overallSent = 0;
}

FileTransfer::~FileTransfer() {
  /*Do not let the object be destroyed before threads are terminated. Call
   * stop() to block threads.*/
  for (auto it = transferThreads.begin(); it != transferThreads.end(); it++) {
    if (it->joinable()) {
      it->join();
    }
  }
}

bool FileTransfer::transfer() {
  try {
    /*Using the control connection (on which the receiver is always listening):
    1. Connection established, the FileReceiver is notified by the 3-way TCP
    Handshake
    2. File name sent, the FileReceiver will be able to ask the user for
    approval
    3. Data connection established
    4. Control connection closed
    */

    TCPSocket s(ip, 50000);
    sendTransferRequest(s);  // throws

    /* Here the other side has accepted the transfer, so it makes sense to start
     * evaluating the time from here.*/
    transferStart.store(std::chrono::system_clock::now());

    tradePort(s);  // throws
    s.Close();

    prepareSockets();  // throws

    if (is_directory(source)) {
      /*Send the directory tree. If some problem occurs, stop.*/
      if (!sendDir()) throw std::domain_error("Can't send the tree. ");

      /*Create threads to transfer files. */
      for (uint16_t i = 0; i < ports.size(); ++i)
        transferThreads.push_back(thread(&FileTransfer::sendFile, this, i));
    } else {
      transferThreads.push_back(thread(&FileTransfer::sendFile, this, 0));
    }

  } catch (std::exception e) {
    success.store(false);
  }
  for (auto it = transferThreads.begin(); it != transferThreads.end(); it++) {
    if (it->joinable()) {
      it->join();
    }
  }
  transferThreads.clear();
  for (auto it = fileSockets.begin(); it != fileSockets.end(); it++)
    it->Close();
  fileSockets.clear();

  return success.load();
}

void FileTransfer::sendTransferRequest(TCPSocket& s) {
  /*Sends immediately the transfer request, containing the directory or the file
   * name. This allows the other side to accept or refuse.*/
  string transferRequestS;
  if (boost::filesystem::is_directory(source))
    transferRequestS = "DIRTREQ";
  else
    transferRequestS = "FILEREQ";

  string transferName = source.filename().string();
  vector<char> message(transferRequestS.begin(), transferRequestS.end());

  uint16_t transferNameSize = htons(transferName.size());

  message.resize(transferRequestS.size() + 2);
  memcpy(message.data() + message.size() - 2, &transferNameSize,
         sizeof(transferNameSize));
  if (s.Send(message) == false)
    throw std::domain_error("Err sending request 1 ");

  message.assign(transferName.begin(), transferName.end());
  if (s.Send(message) == false)
    throw std::domain_error("Err sending request 2 ");
}

void FileTransfer::tradePort(TCPSocket& s) {
  /* Asking to open nThreads data connection (nThreads ports are needed) based
   * on the number of cores. */
  string msg("PORT");
  vector<char> message(msg.begin(), msg.end());
  uint16_t nrThreads;
  if (is_directory(source))
    nrThreads = thread::hardware_concurrency() / 2;
  else
    nrThreads = 1;

  nrThreads = htons(nrThreads);

  message.resize(msg.size() + 2);
  memcpy(message.data() + message.size() - 2, &nrThreads, sizeof(nrThreads));

  if (s.Send(message) == false) throw std::domain_error("Err send 1 ");

  struct timeval t;
  t.tv_sec = 60;
  t.tv_usec = 0;
  if (s.Receive(message, 6, t) == false) throw std::domain_error("Err rcv 1 ");

  /* The response is PORT NrPortsAvailable Port1 Port2 ...
 The host will reply with how many threads it wants to use.*/
  msg.assign(message.begin(), message.begin() + 4);
  if (msg.compare("PORT") != 0)
    throw std::domain_error("Protocol not respected. ");

  memcpy(&nrThreads, message.data() + 4, sizeof(nrThreads));
  nrThreads = ntohs(nrThreads);
  message.clear();
  /* Here all ports are received.*/
  if (s.Receive(message, 2 * nrThreads, t) == false)
    throw std::domain_error("Err recv2 (ports) ");

  ports.resize(nrThreads);
  for (int16_t i = 0; i < nrThreads; i++) {
    memcpy(&ports[i], message.data() + i * 2, sizeof(int16_t));
    ports[i] = ntohs(ports[i]);
  }
}

void FileTransfer::prepareSockets() {
  int8_t tries = 0;
  for (int16_t i = 0; i < ports.size();) {
    try {
      fileSockets.push_back(TCPSocket(ip, ports[i]));
      ++i;
    } catch (...) {
      if (tries <= 1)
        ++tries;
      else
        throw std::domain_error("Can't establish connection. ");
      success.store("false");
      this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  }
}

bool FileTransfer::sendDir() {
  /*Computing directories tree and overall dimension*/

  vector<string> tree;
  vector<char> message;
  auto future_tree = async(&FileTransfer::exploreDir, this);
  auto future_dirSize = async(&FileTransfer::dirSize, this);
  try {
    // Sending SIZE command and waiting for the ACK
    message.push_back('S');
    message.push_back('I');
    message.push_back('Z');
    message.push_back('E');
    message.resize(message.size() + 8);
    overallSize.store(future_dirSize.get());
    uint64_t tempSize = boost::endian::native_to_big(overallSize.load());
    memcpy(message.data() + message.size() - 8, &tempSize, sizeof(tempSize));
    if (!fileSockets[0].Send(message))
      throw std::domain_error("Connection closed. ");

    vector<char> msgV(3);
    string msgS;
    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    if (!fileSockets[0].Receive(msgV, 3, timeout)) {
      throw std::domain_error("Connection closed before SIZE ACK. ");
    }
    msgS.assign(msgV.begin(), msgV.end());
    if (msgS.compare("ACK") != 0)
      throw std::domain_error("Expected ACK for SIZE. ");

    /*Sending the directory tree to the other host and waiting for the ack.*/
    message.clear();
    tree = future_tree.get();
    message = createDirectoryPacket(tree);

    if (!fileSockets[0].Send(message))
      throw std::domain_error("Connection closed. ");

    if (!fileSockets[0].Receive(msgV, 3, timeout)) {
      throw std::domain_error("Connection closed before DIRT ACK. ");
    }
    msgS.assign(msgV.begin(), msgV.end());
    if (msgS.compare("ACK") != 0)
      throw std::domain_error("Expected ACK for DIRT. ");
  } catch (...) {
    return false;
  }
  return true;
}

void FileTransfer::sendFile(uint16_t i) {
  /*This method is executed by separate parallel threads.*/
  boost::filesystem::ifstream file;
  bool flag = true;
  bool isDirectory = boost::filesystem::is_directory(source);
  string relativePath;
  boost::filesystem::path p;
  try {
    while (success.load()) {
      /*Iterates through all files.*/
      if (isDirectory) {
        rdi_protection.lock();
        while (flag) {
          if (rdi == end_rdi) {
            flag = false;
          } else {
            p = rdi->path();
            ++rdi;
            if (!boost::filesystem::is_directory(p)) {
              break;
            }
          }
        }
        rdi_protection.unlock();

        if (flag == false) break;
        relativePath =
            (boost::filesystem::relative(p.generic(), source.generic()))
                .generic()
                .string();
      } else {
        p = source;
        relativePath = source.filename().string();
      }
      uint64_t fileSize = file_size(p);
      if (!isDirectory) overallSize.store(fileSize);

      {
        vector<char> message;
        string temp = "FILE";
        message.assign(temp.begin(), temp.end());
        uint32_t pathlength = relativePath.length();
        message.resize(8);
        pathlength = htonl(pathlength);
        memcpy(message.data() + message.size() - 4, &pathlength,
               sizeof(pathlength));
        message.insert(message.end(), relativePath.begin(), relativePath.end());
        fileSize = boost::endian::native_to_big(fileSize);
        message.resize(message.size() + 8);
        memcpy(message.data() + message.size() - 8, &fileSize,
               sizeof(fileSize));

        fileSize = boost::endian::big_to_native(fileSize);

        if (!fileSockets[i].Send(message))
          throw std::domain_error("Not working. ");

      }  // Clear the stack from some variable

      file.open(p, ios::in | ios::binary);
      if (!file.is_open()) throw std::domain_error("Not working. ");

      uint32_t chunkSize;
      uint32_t padding;
      if (fileSize >= 32768) {
        chunkSize = 32768;
        padding = fileSize % chunkSize;
      } else {
        chunkSize = fileSize;
        padding = 0;
      }
      uint64_t progress = fileSize;
      vector<char> fileChunk(chunkSize);
      while (progress > padding && success.load()) {
        file.read(fileChunk.data(), chunkSize);
        if (!fileSockets[i].Send(fileChunk))
          throw std::domain_error("Not working. ");
        progress = progress - chunkSize;
        overallSent.fetch_add(chunkSize);
      }

      if (padding != 0) {
        fileChunk.resize(padding);
        file.read(fileChunk.data(), padding);
        if (!fileSockets[i].Send(fileChunk))
          throw std::domain_error("Not working. ");
        overallSent.fetch_add(padding);
      }

      file.close();
      if (!isDirectory) break;
    }
    /*No more files available or the connected couple of threads, so quit the
     * connection.*/
    if (success.load()) {
      vector<char> quit;
      quit.push_back('Q');
      quit.push_back('U');
      quit.push_back('I');
      quit.push_back('T');
      fileSockets[i].Send(quit);
    }

  } catch (...) {
    if (file.is_open()) file.close();
    success.store(false);
  }
}

vector<string> FileTransfer::exploreDir() {
  /*Creates the directory three, as a vector of strings.*/
  vector<string> tree;
  if (!exists(source)) throw std::domain_error("The source does not exists. ");

  boost::filesystem::recursive_directory_iterator rdi(source);
  boost::filesystem::recursive_directory_iterator end_rdi;

  tree.push_back(
      relative(source.generic(), source.parent_path()).generic().string());
  for (; rdi != end_rdi; rdi++) {
    if (is_directory((*rdi).path()))
      tree.push_back(relative((*rdi).path(), source).generic().string());
  }
  return tree;
}

uint64_t FileTransfer::dirSize() {
  /*Recursively iterate through all the files to evaluate the overall size. */
  uint64_t size = 0;
  if (!exists(source)) throw std::domain_error("The source does not exists. ");

  boost::filesystem::recursive_directory_iterator rdi(source);
  boost::filesystem::recursive_directory_iterator end_rdi;

  for (; rdi != end_rdi; rdi++) {
    if (!is_directory((*rdi).path())) size += file_size((*rdi).path());
  }
  return size;
}

vector<char> FileTransfer::createDirectoryPacket(vector<string> tree) {
  /*Starting from the directory tree, create a packet that can be interpreted
   * from the other side.*/
  string temp("DIRT");
  vector<char> message;
  message.insert(message.begin(), temp.begin(), temp.end());
  message.insert(message.end(), 4, 0);
  // message.push_back('^');
  for (auto it = tree.begin(); it != tree.end(); it++) {
    message.insert(message.end(), (*it).begin(), (*it).end());
    message.push_back('^');
  }
  uint32_t payloadSize = message.size() - temp.length() - 4;

  payloadSize = htonl(payloadSize);
  memcpy(message.data() + 4, &payloadSize, sizeof(payloadSize));

  string c(message.begin() + 4, message.begin() + 8);

  return message;
}

void FileTransfer::getStatus(transferStatus& t) {
  if (!success.load()) {
    t.progress = FT_ERROR;
    t.speed = 0;
    t.secondsLeft = 0;
    return;
  }

  uint64_t overallSizeTemp = overallSize.load();
  uint64_t overallSentTemp = overallSent.load();
  std::chrono::seconds timeElapsed =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::duration<double>(std::chrono::system_clock::now() -
                                        transferStart.load()));

  /* Getting the progress */
  if (overallSentTemp == 0) {
    t.progress = 0;
  } else {
    t.progress = (overallSentTemp * 100) / overallSizeTemp;
    if (t.progress >= 100) {
      t.progress = FT_COMPLETE;
      t.secondsLeft = 0;
      t.speed = 0;
      return;
    }
  }

  /* Getting the speed */
  if (timeElapsed.count() <= 0) {
    t.speed = 0;
  } else {
    t.speed =
        static_cast<double>(overallSentTemp) / (1048576 * timeElapsed.count());
    if (t.speed < 0) t.speed = 0;
  }

  if (t.speed == 0)
    t.secondsLeft = 0;
  else
    t.secondsLeft = static_cast<uint32_t>((overallSizeTemp - overallSentTemp) /
                                          (1048576 * t.speed));
}

void FileTransfer::stop() { success.store(false); }
