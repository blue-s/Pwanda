//#using <System.dll>
//#include <stdio.h>
//#include <windows.h>
//#include <tchar.h>
//#include <locale.h>
//#include <shlobj.h>                  // for IsUserAnAdmin() function.
//#pragma comment(lib, "shell32.lib")
//
//
//
//DWORD FindProcessId(const char *processname)
//{
//    HANDLE hProcessSnap;
//    PROCESSENTRY32 pe32;
//    DWORD result = NULL;
//
//    // Take a snapshot of all processes in the system.
//    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//    if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);
//
//    pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT
//
//    // Retrieve information about the first process,
//    // and exit if unsuccessful
//    if (!Process32First(hProcessSnap, &pe32))
//    {
//        CloseHandle(hProcessSnap);          // clean the snapshot object
//        printf("!!! Failed to gather information on system processes! \n");
//        return(NULL);
//    }
//
//    do
//    {
//        printf("Checking process %ls\n", pe32.szExeFile);
//        if (0 == strcmp(processname, pe32.szExeFile))
//        {
//            result = pe32.th32ProcessID;
//            break;
//        }
//    } while (Process32Next(hProcessSnap, &pe32));
//
//    CloseHandle(hProcessSnap);
//
//    return result;
//}
//
//int main()
//{   
//   // Get the current process.    
//   Process^ currentProcess = Process::GetCurrentProcess();
//
//   // Get all processes running on the local computer.
//   array<Process^>^localAll = Process::GetProcesses();
//
//   // Get all instances of Notepad running on the local computer.
//   // This will return an empty array if notepad isn't running.
//   array<Process^>^localByName = Process::GetProcessesByName("notepad");
//
//   // Get a process on the local computer, using the process id.
//   // This will throw an exception if there is no such process.
//   Process^ localById = Process::GetProcessById(1234);
//
//
//   // Get processes running on a remote computer. Note that this
//   // and all the following calls will timeout and throw an exception
//   // if "myComputer" and 169.0.0.0 do not exist on your local network.
//
//   // Get all processes on a remote computer.
//   array<Process^>^remoteAll = Process::GetProcesses("myComputer");
//
//   // Get all instances of Notepad running on the specific computer, using machine name.
//   array<Process^>^remoteByName = Process::GetProcessesByName( "notepad", "myComputer" );
//
//   // Get all instances of Notepad running on the specific computer, using IP address.
//   array<Process^>^ipByName = Process::GetProcessesByName( "notepad", "169.0.0.0" );
//
//   // Get a process on a remote computer, using the process id and machine name.
//   Process^ remoteById = Process::GetProcessById( 2345, "myComputer" );
//}