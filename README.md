# emc_test_cpp

The main solution is located in msvc/EMC.sln

Once generated the executable will expect 2 parameters:
1. The URL 
2. The search term

To build the solution, it will probably require the installation of a third party library, Casablanca. The installation is pretty straightforward, it can be installed with the NuGet package manager. The instruction can be found at this URL: https://casablanca.codeplex.com/wikipage?title=Using%20NuGet%20to%20add%20the%20C%2b%2b%20REST%20SDK%20to%20a%20VS%20project

Once the solution is built, you can run the executable via the command line:
> bootstrap_exe.exe http://www.google.com search

Expected results:
_____________________________________
        __          ___    __________
\    ___) |        |  /  __)    _
 |  (__   |  |\/|  | |  /     _( )_
 |   __)  |  |  |  | | |     (__ __)
 |  (___  |  |  |  | |  \__    (_)
/       )_|  |__|  |__\    )_________
_____________________________________
Looking up for the term 'search' @ http://www.google.com
[LOG] Received response code: 200
The word following the lookup term is: '("&ei=")){var'

There is also a test project that is not part of the main solution, since I've changed the interface quite a bit after the first draft. It uses BDD with the Catch framework.

If there are any questions or comment, do not hesitate to contact me.
Thanks!
