def readEnvFile(fname):
    """Reads a bash file with void environment and returns the environment
    at the end of the execution."""
    proc = sp.Popen('. '+fname+' &> /dev/null; env', \
      shell=True, stdout=sp.PIPE, env={})
    lines = proc.stdout.read().split('\n')[:-1]
    env = dict([l.split('=', 1) for l in lines])

    for k in ('SHLVL', 'PWD', '_'):
        if env.has_key(k):
            del env[k]
    return env

def parseConf(fname):
    input = file(fname).read()
    
    tests = {}
    for line in input.split('\n'):
        line = line.strip()
        spl = [i.strip() for i in shlex.split(line)]
        if len(spl) < 2:
            continue
        if line[0] == '#':
            continue
        env = {}
        skip = []
        change = {}
        descr = None
        fileenv = {}

        # Interpret arguments
        for var in spl[2:]:

            # if begins with '-': skip implementation
            if var[0] == '-':
                skip.append(var[1:])

            # if key:value, substitute pkg-config dependency
            elif ':' in var and not '=' in var:
                c_0, c_1 = var.split(':', 1)
                change[c_0] = c_1

            # if descr|text set description (for future use)
            elif var[:6] == 'descr|':
                descr = var[6:]

            # if @file: read bash script and set env
            elif var[0] == '@':
                fileenvNew = readEnvFile(pjoin(cfg.curdir, var[1:]))
                fileenv = dict( fileenv.items() + fileenvNew.items() )
                del fileenvNew

            # Otherwise, assume key=value syntax
            else:
                e_0, e_1 = var.split('=', 1)
                env[e_0] = e_1

        # Set environment (argument overrides bash file)
        env = dict( fileenv.items() + env.items() )

        try:
            # Insert test
            avail = available_packages(spl[1])[-1]
            tests[spl[0]] = {'package':avail , 'emergeenv':env, 'skip':skip, \
              'requires':change, 'descr':descr}
        except:
            # Or trigger an non-fatal error
            sys.stderr.write('Error: package ' + spl[1] + ' not found\n')
    return tests