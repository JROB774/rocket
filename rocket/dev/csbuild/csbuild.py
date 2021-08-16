#!/usr/bin/env python3

import subprocess
import platform
import vswhere
import json
import os
import sys
import ntpath
import datetime
import time

from argparse import ArgumentParser

g_start_time = 0
g_project_name = ""
g_config_name = ""
g_cache_path = ""
g_log_file_name = ""

def empty_config():
    config = {}
    config["dependencies"] = []
    config["output"] = ""
    config["cpp"] = ""
    config["debug"] = False
    config["unity"] = False
    config["compiler_params"] = ""
    config["linker_params"] = ""
    config["include_dirs"] = []
    config["library_dirs"] = []
    config["defines"] = []
    config["libraries"] = []
    config["headers"] = []
    config["sources"] = []
    return config

def generate_config(config):
    print("Generating build config...")
    file_name = os.path.join(g_cache_path, g_config_name + "/build.config")
    if not os.path.exists(os.path.dirname(file_name)):
        os.makedirs(os.path.dirname(file_name))
    with open(file_name, "w") as file:
        json.dump(config, file, indent=4)

def openfile(filename, mode):
    pathname = os.path.dirname(filename)
    if not os.path.exists(pathname):
        os.makedirs(pathname)
    return open(filename, mode)

# Logger taken and modified from here: https://stackoverflow.com/a/14906787
class logger(object):
    def __init__(self):
        self.terminal = sys.stdout
        self.log = openfile(g_log_file_name, "w")
    def write(self, message):
        self.terminal.write(message)
        self.log.write(message)
    def flush(self):
        pass
    def fileno(self):
        return self.log.fileno()

def pathleaf(path):
    head, tail = ntpath.split(path)
    return tail or ntpath.basename(head)

def printheader(header, border="-"):
    print(border * 80)
    print(header)
    print(border * 80)

def terminate(success = False):
    if success:
        printheader("success")
    else:
        printheader("failure")
    endtime = time.time()
    print("Build took: %ss" % (endtime - g_start_time))
    sys.exit()

def getvsdevcmd():
    return os.path.realpath(os.path.join(vswhere.get_latest_path(), "Common7/Tools/vsdevcmd.bat"))

# Function taken and modified from here: https://stackoverflow.com/a/27639470
def execute(command, env=None):
    popen = subprocess.Popen(command, stdout=subprocess.PIPE, env=env)
    lines_iterator = iter(popen.stdout.readline, b"")
    while popen.poll() is None:
        for line in lines_iterator:
            nline = line.rstrip()
            print(nline.decode("latin"), flush=True) # Yield line.
    return popen.returncode

def buildwin32(config, name, timestamps, rebuild):
    # If there are timestamps check them to see what files actually need building.
    sources = []
    if timestamps: # @TEMPORARY: Force a full rebuild if any of the headers have changed!
        for header in config["headers"]:
            if header not in timestamps:
                rebuild = True
            elif os.path.getmtime(header) > timestamps[header]:
                rebuild = True
    if rebuild or not timestamps:
        sources = config["sources"]
    elif timestamps:
        for source in config["sources"]:
            if source not in timestamps:
                sources.append(source)
            elif os.path.getmtime(source) > timestamps[source]:
                sources.append(source)
    if not sources:
        print("Project up-to-date!")
        return True
    # Print some information about the build process.
    print("Environment location: %s" % vswhere.get_latest()["installationPath"])
    print("Environment: %s (%s)" % (vswhere.get_latest()["displayName"], vswhere.get_latest()["installationVersion"]))
    # Store the MSVC dev environment so we don't have to keep calling VsDecCmd which is fairly slow.
    devenv = {}
    print("Searching for MSVC dev environment...")
    envfile = os.path.normpath(os.path.join(g_cache_path, "devenv.json"))
    if os.path.isfile(envfile):
        print("Found existing dev environment!")
        with open(envfile, "r") as file:
            devenv = json.loads(file.read())
        if not devenv:
            print("Dev environment is empty...")
    if not devenv:
        print("Searching for VsDevCmd...")
        vsdevcmd = getvsdevcmd()
        if not os.path.isfile(vsdevcmd):
            print("[error]: Could not find VsDevCmd file!")
            terminate()
        print("VsDevCmd found!")
        setupname = os.path.normpath(os.path.join(g_cache_path, name + "/setup.bat"))
        with openfile(setupname, "w") as file:
            file.write("call \"" + vsdevcmd + "\" -no_logo -arch=amd64\n")
            file.write("set")
        p = subprocess.Popen(["cmd", "/v:on", "/q", "/c", setupname], stdout=subprocess.PIPE, universal_newlines=True)
        stdout, stderr = p.communicate()
        for line in stdout.splitlines():
            data = line.split("=", 1)
            devenv[data[0]] = data[1]
        with open(envfile, "w") as file:
            json.dump(devenv, file, indent=4)
        print("Environment saved to: %s" % envfile)
    # Generate a batch file for executing the build.
    print("Generating build script...")
    scriptname = os.path.normpath(os.path.join(g_cache_path, name + "/build.bat"))
    binary = os.path.realpath(config["output"])
    cmds = []
    cmds.append("@echo off")
    cmds.append("setlocal")
    cmds.append("if not exist %s mkdir %s" % (os.path.dirname(binary), os.path.dirname(binary)))
    for file in sources:
        cmd = "cl -nologo -Zc:__cplusplus -c "
        if config["cpp"]:
            cmd += "-std:" + config["cpp"]  + " "
        if config["debug"]:
            cmd += "-Z7 "
        cmd += config["compiler_params"] + " "
        cmd += "-Fo" + os.path.dirname(scriptname) + "/ " # Need trailing slash.
        for incdir in config["include_dirs"]:
            cmd += "-I " + incdir + " "
        for define in config["defines"]:
            cmd += "-D " + define + " "
        cmds.append(cmd + file)
        cmds.append("if %ERRORLEVEL% neq 0 exit /B 1")
    objs = []
    for file in config["sources"]:
        objs.append(os.path.join(os.path.dirname(scriptname), os.path.splitext(pathleaf(file))[0] + ".obj"))
    buildtype = os.path.splitext(config["output"])[1]
    link = ""
    if buildtype == ".exe":
        link = "link -nologo "
        if config["debug"]:
            link += "-debug "
    elif buildtype == ".lib":
        link = "lib -nologo "
    else:
        print("[error] Unknown build type '" + buildtype + "'!")
        terminate()
    link += config["linker_params"] + " "
    for libdir in config["library_dirs"]:
        link += "-libpath:" + libdir + " "
    for lib in config["libraries"]:
        link += lib + " "
    for obj in objs:
        link += obj + " "
    link += "-out:" + binary
    cmds.append(link)
    cmds.append("if %ERRORLEVEL% neq 0 exit /B 1")
    cmds.append("endlocal")
    with openfile(scriptname, "w") as file:
        file.write('\n'.join([i for i in cmds[:]]))
    if not os.path.isfile(scriptname):
        print("[error] Failed to generate build script!")
        terminate()
    print("Build command saved to: %s" % scriptname)
    # Launch the build script to start the build process.
    print("Launching build...")
    return execute(scriptname, devenv) == 0

def debugwin32(name, config):
    # Check that the executable exists otherwise we have nothing to debug.
    executable = os.path.realpath(config["output"])
    if not os.path.isfile(executable):
        print("[error]: Executable does not exist!")
        terminate()
    # Print some information about the debug process.
    print("Executable: %s" % executable)
    print("Environment location: %s" % vswhere.get_latest()["installationPath"])
    print("Environment: %s (%s)" % (vswhere.get_latest()["displayName"], vswhere.get_latest()["installationVersion"]))
    # Generate a batch file for executing the debugger.
    print("Generating debug script...")
    vsdevcmd = getvsdevcmd()
    if not os.path.isfile(vsdevcmd):
        print("[error]: Could not find VsDevCmd.bat file!")
        terminate()
    scriptname = os.path.normpath(os.path.join(g_cache_path, name + "/debug.bat"))
    cmds = []
    cmds.append("@echo off")
    cmds.append("setlocal")
    cmds.append("call \"" + vsdevcmd + "\" -no_logo -arch=amd64")
    cmds.append("devenv " + executable)
    cmds.append("endlocal")
    with openfile(scriptname, "w") as file:
        file.write('\n'.join([i for i in cmds[:]]))
    if not os.path.isfile(scriptname):
        print("[error] Failed to generate debug script!")
        terminate()
    print("Debug command saved to: %s" % scriptname)
    # Launch the debug script to start the developer environment.
    print("Launching debugger...")
    subprocess.Popen(scriptname) # Don't wait.

def script(script_name, script_locals):
    global g_config_name
    g_config_name = os.path.basename(os.path.dirname(os.path.realpath(script_name)))
    printheader("script (" + g_config_name + ")")
    if not os.path.isfile(script_name):
        print("[error]: Could not find a build script at the current location!")
        terminate()
    print("Entering build script...")
    cwd = os.getcwd()
    os.chdir(os.path.dirname(os.path.realpath(script_name)))
    exec(open(os.path.basename(script_name)).read(), globals())
    os.chdir(cwd)
    config_name = os.path.join(g_cache_path, g_config_name + "/build.config")
    if not os.path.isfile(config_name):
        print("[error]: Failed to generate build config file!")
        terminate()
    print("Config saved to: %s" % os.path.normpath(config_name))
    if script_locals["cs_args"]:
        print("Arguments used: %s" % " ".join(script_locals.args))
    # If there are dependencies then run the script for the dependencies.
    config = empty_config()
    with open(config_name, "r") as file:
        config = json.loads(file.read())
        for dep in config["dependencies"]:
            script(dep, script_locals)
    return config

def build(config, rebuild):
    final_rebuild = rebuild
    # If a project is a unity build then it should always be rebuilt.
    if not final_rebuild:
        final_rebuild = config["unity"]
    # If a project has dependencies then go through and build them in reverse order, otherwise just build the project
    if config["dependencies"]:
        for dep in list(reversed(config["dependencies"])):
            config_name = os.path.join(g_cache_path, os.path.basename(os.path.dirname(os.path.realpath(dep))) + "/build.config")
            dep_config = empty_config()
            with open(config_name, "r") as file:
                cwd = os.getcwd()
                os.chdir(os.path.dirname(os.path.realpath(dep)))
                dep_config = json.loads(file.read())
                if build(dep_config, rebuild): # If a dependency changed then rebuild the dependent projects.
                    final_rebuild = True
                else:
                    return False
                os.chdir(cwd)
    # Load the timestamps if they're present so we can check what files need rebuilding.
    name = os.path.splitext(os.path.basename(config["output"]))[0]
    timestamps = {}
    timestamps_name = os.path.join(g_cache_path, name + "/timestamps.json")
    if os.path.exists(timestamps_name):
        with open(timestamps_name, "r") as file:
            timestamps = json.loads(file.read())
    # Do the build process for the current platform.
    printheader("build (" + name + ")")
    system = platform.system()
    result = False
    if system == "Windows":
        result = buildwin32(config, name, timestamps, final_rebuild)
    else:
        print("[error]: Current platform %s is unsupported!" % system)
        terminate()
    # For all of the header and source files store their timestamps that can be used for partial rebuild.
    timestamps = {}
    if result: # Only update timestamps if the build succeeded.
        for file in config["headers"]:
            timestamps[file] = os.path.getmtime(file)
        for file in config["sources"]:
            timestamps[file] = os.path.getmtime(file)
    if timestamps:
        with open(timestamps_name, "w") as file:
            json.dump(timestamps, file, indent=4)
    return result

def launch(config):
    printheader("launch")
    buildtype = os.path.splitext(config["output"])[1]
    if buildtype != ".exe":
        print("[error]: Cannot launch non-executable!")
        terminate()
    executable = os.path.realpath(config["output"])
    if not os.path.exists(executable):
        print("[error]: Executable does not exist!")
        terminate()
    print("Executable: %s" % executable)
    print("Launching executable...")
    subprocess.Popen(executable, cwd=os.path.dirname(executable)) # Don't wait.

def debug(config):
    printheader("debug")
    name = os.path.splitext(os.path.basename(config["output"]))[0]
    buildtype = os.path.splitext(config["output"])[1]
    if buildtype != ".exe":
        print("[error]: Cannot debug non-executable!")
        terminate()
    system = platform.system()
    if platform.system() == "Windows":
        debugwin32(name, config)
    else:
        print("[error]: Current platform %s is unsupported!" % system)
        terminate()

def main():
    # Parse the csbuild command line arguments that have been passed in.
    parser = ArgumentParser(description="build and manage cross-platform c/c++ projects")
    parser.add_argument("-f", "--file", help="specify a custom build.script file name to use", dest="file", default="")
    parser.add_argument("-b", "--build", help="build the project", action="store_true", dest="build", default=False)
    parser.add_argument("-r", "--rebuild", help="rebuild the project", action="store_true", dest="rebuild", default=False)
    parser.add_argument("-l", "--launch", help="launch the project", action="store_true", dest="launch", default=False)
    parser.add_argument("-d", "--debug", help="debug the project", action="store_true", dest="debug", default=False)
    parser.add_argument("-a", "--args", help="extra arguments to pass to the build.script file", nargs="*", default=[])
    args = parser.parse_args()

    # Set the script name to the default, if a custom name is provided then use that.
    script_name = "build.script"
    if args.file:
        script_name = args.file

    # Initial setup of some global variables.
    global g_start_time
    global g_project_name
    global g_config_name
    global g_cache_path
    global g_log_file_name

    g_start_time = time.time()
    g_project_name = os.path.basename(os.path.dirname(os.path.realpath(script_name)))
    g_config_name = g_project_name
    g_cache_path = os.path.join(os.path.normpath(os.path.join(os.path.dirname(os.path.realpath(__file__)), "__cscache__")), g_project_name)
    g_log_file_name = os.path.join(g_cache_path, "output.log")

    # Make sure the cache directory for this project exists.
    if not os.path.exists(g_cache_path):
        os.makedirs(g_cache_path)

    # Setup our custom logger that writes to both log file and terminal.s
    sys.stdout = logger()

    # Print some useful startup information.
    printheader("csbuild", "=")
    print("Program location: %s" % os.path.realpath(__file__))
    print("Log location: %s" % g_log_file_name)
    print("Script location: %s" % os.path.realpath(script_name))
    print("Project Name: %s" % g_project_name)
    print("Start time: %s" % datetime.datetime.now())
    print("Platform: %s" % platform.platform())

    # Check that the build script exists, use it to generate a build config and run custom build script code.
    config = script(script_name, { "cs_args": args.args, "cs_build": args.build, "cs_launch": args.launch, "cs_debug": args.debug })

    # If any extra tasks were specified then and carry those out.
    result = True
    if args.build or args.rebuild:
        result = build(config, args.rebuild)
    if args.launch:
        launch(config)
    if args.debug:
        debug(config)

    # Finish execution.
    terminate(result)

if __name__ == "__main__":
    main()
