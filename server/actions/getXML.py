from xml.dom import minidom
import cgi, re, sys
from pprint import pprint
doc = minidom.Document()

# Define form (which is already defined, but...)
if not locals().has_key('form'):
    form = cgi.FieldStorage()

# Begins 
cases = {}
patterns = []

# Help function that ensure that the case contains the needed lists 
def ED(n):
    if not cases.has_key(n):
        cases[n] = {}
        for l in ('depend', 'emerge', 'compile', 'run', 'skip', 'skipre'):
            cases[n][l] = []
    return True

# Define patterns and response function
patterns.append((
    re.compile(r'id([0-9]*)'),
    lambda G, v : ED(G[0]) and cases[G[0]].update(id=v)
))
patterns.append((
    re.compile(r'package([0-9]*)'),
    lambda G, v : ED(G[0]) and cases[G[0]].update(package=v)
))

for envname in ('depend', 'emerge', 'compile', 'run'):
    patterns.append((
        re.compile('('+envname+')' + r'([0-9]*)_(.*)'),
        lambda G, v : ED(G[1]) and cases[G[1]][G[0]].append((G[2], v))
    ))

patterns.append((
    re.compile(r'skip([0-9]*)_(.*)'),
    lambda G, v : ED(G[0]) and \
      cases[G[0]]['skipre' if v=='1' else 'skip'].append(G[1])
))

# Parse the form arguments and generate the dictionary
for key in form:
    for p, f in patterns:
        match = p.match(key)
        
        if match is None:
            continue
        
        groups = match.groups()
        value = form.getvalue(key)
        f(groups, value)

        break


# THE GENERATION OF THE XML DOCUMENT BEGINS

# Main elements
main = doc.createElement('numbench')
operations = doc.createElement('operations')
testcases = doc.createElement('testcases')

# Operations element
operations.setAttribute('module', form.getvalue('moduleName'))
operations.appendChild(doc.createTextNode(
        form.getvalue('moduleArgs')))

# Generate case elements
for case in cases.values():
    caseTag = doc.createElement('case')
    
    # ID and package
    try:
        caseTag.setAttribute('id', case['id'])
        
        pkgTag = doc.createElement('pkg')
        pkgTag.appendChild(doc.createTextNode(case['package']))
        caseTag.appendChild(pkgTag)
        
    except KeyError:
        continue
    
    # Environments
    for Eshort in ('depend', 'emerge', 'compile', 'run'):
        Elong = Eshort + 'env'
        if (len(case[Eshort]) == 0):
            continue
        
        envTag = doc.createElement(Elong)
        for varName, varVal in case[Eshort]:
            varTag = doc.createElement('var')
            varTag.setAttribute('name', varName)
            varTag.appendChild(doc.createTextNode(varVal))
            envTag.appendChild(varTag)
        
        caseTag.appendChild(envTag)
        
    # Skip
    for skip in case['skipre']:
        skipTag = doc.createElement('skip')
        skipTag.setAttribute('type', 'regexp')
        skipTag.appendChild(doc.createTextNode(skip))
        
        caseTag.appendChild(skipTag)
    
    for skip in case['skip']:
        skipTag = doc.createElement('skip')
        skipTag.appendChild(doc.createTextNode(skip))
        
        caseTag.appendChild(skipTag)
    
    # Case is done: append it to the testcases
    testcases.appendChild(caseTag)


# Put things together
main.appendChild(operations)
main.appendChild(testcases)

doc.appendChild(main)


print cgi.escape(doc.toprettyxml('    '))
#pprint(cases)
