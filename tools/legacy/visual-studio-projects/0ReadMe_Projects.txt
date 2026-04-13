This directory contains a legacy set of Visual Studio 2008 build projects
for the inherited SIMH code base. It is retained primarily for historical
reference, and may be removed in a future cleanup. The supported Windows
build path for this repository is the CMake-based workflow.

When used experimentally (with Visual Studio Express 2008 or a later
Visual Studio version) it populates a directory tree under the BIN
directory of the distribution for temporary build files and produces
resulting executables in the BIN/NT/Win32-Debug or BIN/NT/Win32-Release
directories (depending on whether you target a Debug or Release build).

These projects, when used with Visual Studio 2008, will produce Release 
build binaries that will run on Windows versions from XP onward.  Building
with later versions of Visual Studio will have different Windows version
compatibility.

These project files expect that various dependent packages are available
in a directory parallel to the source tree.

For Example, the directory structure should look like:

    .../simh/sim-master/simulators/VAX/vax_cpu.c
    .../simh/sim-master/src/core/scp.c
    .../simh/sim-master/tools/legacy/visual-studio-projects/Simh.sln
    .../simh/sim-master/tools/legacy/visual-studio-projects/VAX.vcproj
    .../simh/sim-master/BIN/Nt/Win32-Release/vax.exe
    .../simh/windows-build/pthreads/pthread.h
    .../simh/windows-build/winpcap/WpdPack/Include/pcap.h
    .../simh/windows-build/libSDL/SDL2-2.0.3/include/SDL.h

If you have a command line version of git installed in your environment
then the windows-build repository will be downloaded and updated 
automatically.  Git for windows can be downloaded from:

    https://git-scm.com/download/win
    
If git isn't available, then the contents of the windows-build 
directory can be downloaded from:

    https://github.com/simh/windows-build/archive/windows-build.zip

Download and extract the contents of this zip file into the appropriate 
place in your directory structure.  You do not need to do anything else
but have this directory properly located.

Network devices are capable of using pthreads to enhance their performance.  
To realize these benefits, you must build the desire simulator with 
USE_READER_THREAD defined.  The relevant simulators which have network 
support are all of the VAX simulators, the PDP11 simulator and the various
PDP10 simulators.

Additionally, simulators which contain devices that use the asynchronous
APIs in sim_disk.c and sim_tape.c can also achieve greater performance by
leveraging pthreads to perform blocking I/O in separate threads.  Currently
the simulators which have such devices are all of the VAX simulators and 
the PDP11.  To achieve these benefits the simulators must be built with 
SIM_ASYNCH_IO defined.

The project files in this directory build these simulators with support for
both network and asynchronous I/O.

To build any of the supported simulators you should open the simh.sln file 
in this directory.

The installer for Visual Studio 2008 SP1 is available from:

http://download.microsoft.com/download/E/8/E/E8EEB394-7F42-4963-A2D8-29559B738298/VS2008ExpressWithSP1ENUX1504728.iso

Then install Visual Studio Express Visual C++ by executing VCExpress\setup.exe 
on that DVD image.  No need to install "Silverlight Runtime" or 
"Microsoft SQL Server 2008 Express Edition".  Depending on your OS Version 
you may be prompted to install an older version of .NET Framework which should 
be installed.

Note: VS2008 can readily coexist on Windows systems that also have later 
versions of Visual Studio installed.

If you are using a version of Visual Studio beyond Visual Studio 2008, then 
your later version of Visual Studio will automatically convert the Visual 
Studio 2008 project files.  You should ignore any warnings produced by the 
conversion process.

If you want to experiment with these project files, open the Simh.sln file
in this directory directly from Visual Studio. The old command-line helper
script is no longer maintained as a supported build path.
