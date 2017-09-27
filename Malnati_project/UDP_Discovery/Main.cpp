#include "UDP_Discover.h"
using namespace std;

int main() {
	cout << "Starting... "<<endl;
	{//I need it to go out of scope to test the destructor
		UDP_Discover udw("MassimoFisso_____", "3.png");
		cout << "Ora deve partire attivo" << endl;
		udw.run(UDS_ACTIVE);
		getchar();

		cout << "Ora deve nascondersi: " << udw.changeMode(UDS_HIDDEN) << endl;
		getchar();

		cout << "Ora deve riprendere" << endl;
		udw.changeMode(UDS_ACTIVE);
		getchar();

		cout << "Ora deve fermarsi" << endl;
		udw.stop();
		getchar();

		cout << "Ora deve riprendere mascherato" << endl;
		udw.run(UDS_HIDDEN);
		cout << "Al prossimo invio verr� distrutto." << endl;
		getchar();
			
	}
	cout << "If I am here, there is no more UDP." << endl;
	getchar();
return 0;
}
