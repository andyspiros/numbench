import re

def interpret(string, info):
    pattern = r'(.*?)<!--PY(.*?)PY-->(.*)'

    while True:
        m = re.match(pattern, string, re.DOTALL)
        if not m:
            print string
            break

        g = m.groups()
        print g[0]
        try:
            print info.__getattribute__(g[1].strip())()
        except:
            print "<strong>Error: no attribute " + g[1] + "</strong>"
        string = g[2]
