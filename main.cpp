#include <iostream>
#include <windows.h>
#include "Helper.h"
#include "KeyConstants.h"
#include "Base64.h"
#include "IO.h"
using namespace std;


int main(){

    /* Hide any pop-up message and window
     * for this part we need the header windows.h
     */
    MSG message;
    while (GetMessage (&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return 0;
}
