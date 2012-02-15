from os.path import basename

def parseInput(fname):
    term = basename(fname).rsplit('.')[-1]
    
    if term.lower() == 'xml':
        import xmlinput as parser
    else:
        import oldinput as parser
    
    return parser.parseConf(fname)