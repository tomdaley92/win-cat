# WinCat
**v1.04**


A simple TCP/IP network debugging utility for Windows.
Inspired by the traditional nc we all know and love.

**[Downloads](https://github.com/tomdaley92/WinCat/releases)**

## Usage
    wc [-lkszhv] [--e filename] [--c string] [host] [port]
    
    -l      Listen for incoming connections. It is an error to
            use this option with a host specified
    -k      Keep listening. Forces wc to stay listening 
            for another connection after its current
            connection is completed. It is an error to use
            this option without -l.

    -s      Specify host(s) on the network to send ICMP echo
            requests. It is an error to use this option with
            any other options specified.
            e.g.    wc -s 192.168.1.0/24

    -z      Specify port(s) on the host to scan for listening
            daemons using the connect() call. It is an error
            to use this option with any other options
            specified.
            e.g.    wc -z localhost 1-200

    --c     Specify commands to pass to "cmd /c" for
            execution. It is an error to use this option
            with --e, -s, or -z.
            e.g.    host A (10.0.0.2): wc -l --c whoami 8118
                    host B (10.0.0.3): wc 10.0.0.2 8118

    --e     Specify filename to execute after connect
            (use with caution). It is an error to use this
            option with --c, -s, or -z.
            e.g.    host A (10.0.0.2): wc -lk --e cmd 8118
                    host B (10.0.0.3): wc 10.0.0.2 8118

    -h      Print this help page.

    -v      Print version information.

    host    Can be a numerical address or a symbolic
            hostname. If the -s option is specified, CIDR
            notation (IPv4 only) can be used to specify a
            range of hosts.

    port    Port must be single integer. If the -z option
            is specified, a range of ports can be used instead.

## Building on Windows
Microsoft's Visual C++ Build Tools 
(vcvarsall/cl/nmake) are assumed to be 
installed and added to PATH.
1) Open the command prompt and navigate 
   to the WinCat directory.
2) Type `vcvarsall x86` to load the 
   windows development environment.
3) Type `nmake`.
