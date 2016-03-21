#ifndef HELPER_H
#define HELPER_H


#include <ctime>        //use it for timeouts in mail sending purposes
#include <string>
#include <sstream>      // converting various types to string
#include <fstream>

namespace Helper {
    template <class T>  // instead of "class T" we can use "typedef T"

    // Convert anything to a string using a generic type with a reference to a constant.
    // For any data type it accepts, the compiler will creates a separate function for each
    // data type, with exactly the same body
    std::string ToString (const T &);

    // A data type for representing date and time.
    // !!! Mind that struct can be accessed by any other functions in the application
    struct DateTime {

        // Create an object DateTime() on the local machine (on which the keyLogger runs)
        DateTime() {

            time_t ms;
            time (&ms);   // Take the system time and put it in the variable "ms"

            /*
             * info = pointer to a struct tm; type from ctime library
             * localtime will receive the address of the variable ms and will return a value
             * Then use that value and state that pointer info will point to that particular value
             */
            struct tm *info = localtime(&ms);
            /* D = attribute from DateTime used for "Day"
             * m = attribute from DateTime used for "Month". It will start from 0 so we should add 1 to it
             * y = attribute from DateTime used for "Year"
             * M = attribute used for "Minutes"
             * H = attribute used for "Hour"
             * S = attribute used for "Seconds"
             */
            D = info -> tm_mday;
            m = info -> tm_mon + 1;
            y = 1900 + info -> tm_year;
            M = info -> tm_min;
            H = info -> tm_hour;
            S = info -> tm_sec;
        }

        // Second constructor; Needed for additional functionality
         DateTime(int D, int m, int y, int H, int M, int S): D(D), m(m), y(y), H(H), M(M), S(S) {}

        // Set date, but time will be 0
        DateTime(int D, int m, int y): D(D), m(m), y(y), H(0), M(0), S(0) {}

        // Method to READ and return the current DateTime
        DateTime Now () const {
            return DateTime ();
        }

        int D, m, y, H, M, S;

        // Format our date to a format like DD.mm.YYYY
        std::string GetDateString() const {
            return std::string(D < 10 ? "0" : "") + ToString(D) + std::string (m < 10 ? "0" : "") + ToString(m) + "." + ToString(y);
        }

        // Format our date to a format like HH:MM:SS
        // Default separator is ":"
        std::string GetTimeString(const std::string &sep = ":") const {
            return std::string (H < 10 ? "0" : "") + ToString(H) + sep +
                   std::string (M < 10 ? "0" : "") + ToString(M) + sep +
                   std::string (S < 10 ? "0" : "") + ToString(S);
        }

        // Method to combine the 2 methods from above and their results
        std::string GetDateTimeString (const std::string &sep = ":") const {
            return GetDateString() + " " + GetTimeString(sep);
        }
    }; // struct DateTime

    // A generic function to convert anything to a string
    template <class T>

    /* This function will only accept a type if that
     * type supports the insertion operator "<<"
     * Otherwise, this will not compile.
     */
    std::string ToString (const T & e) {
        // Variable "s" of type output stringstream
        std::ostringstream s;
        s << e;

        return s.str();
    }

    /* Optional method for debugging the application.
     * If anything went wrong, we will use this method to see why.
     */
    void WriteAppLog (const std::string &s) {
        std::ofstream file ("Applog.txt", std::ios::app); //ios = input output stream; app = append
        file << "[" << Helper::DateTime().GetDateTimeString() << "\n" << s << std::endl << "\n";
        file.close();
    }
}

#endif // HELPER_H
