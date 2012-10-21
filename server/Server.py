import sys, os, cgi, string, random, Cookie
from StringIO import StringIO
from os.path import dirname, realpath, join as pjoin
from SimpleHTTPServer import SimpleHTTPRequestHandler

chars = string.ascii_letters + string.digits
sessionDict = {} # dictionary mapping session id's to session objects

class SessionElement(object):
    """Arbitrary objects, referenced by the session id"""
    d = {}

def generateRandom(length):
    """Return a random string of specified length (used for session id's)"""
    return ''.join([random.choice(chars) for _ in range(length)])

class HTTP_REDIRECTION(Exception):
    pass


class MyRequestHandler(SimpleHTTPRequestHandler):

    pages = pjoin(dirname(realpath(__file__)), 'pages/')

    def getFile(self, localpath):
        try:
            fs = file(localpath, 'rb')
            ctype = self.guess_type(localpath)
        except:
            self.send_error(404, "File " + localpath + " not found")
            return

        self.send_response(200)
        self.send_header("Content-type", ctype)
        fss = os.fstat(fs.fileno())
        self.send_header("Content-Length", str(fss[6]))
        self.send_header("Last-Modified", self.date_time_string(fss.st_mtime))
        self.end_headers()
        self.copyfile(fs, self.wfile)
        fs.close()
    
    
    def writeContent(self, text, ctype='text/plain'):
        self.send_response(200)
        self.send_header("Content-type", ctype)
        self.send_header("Content-Length", len(text))
        self.end_headers()
        self.wfile.write(text)

    def checkLogin(self):
        self.cookie = Cookie.SimpleCookie()
        if self.headers.has_key('cookie'):
            self.cookie = Cookie.SimpleCookie(self.headers.getheader("cookie"))
            
        sd = self.Session().d
        print 'Session dictionary:', sd
        if 'login' not in sd and not self.path.startswith('/login'):
            print "You must login!"
            print " * in dict:", 'login' not in sd
            print " * path:", not self.path.startswith('/login')
            sd['requested'] = self.path
            
            self.send_response(301)
            for m in self.cookie.values():
                self.send_header('Set-Cookie', m.output(header='').lstrip())
            self.send_header('Location', '/login.html')
            self.end_headers()
            return False
        
        print "Login already done"
        return True


    def do_GET(self):
        print "\nRequested path", self.path
        
        if not self.checkLogin():
            return
        
        print "Ok, we are in", self.path
        
        path = self.path

        if path == '/':
            return self.getFile(pjoin(self.pages, 'index.html'))

        if path.startswith('/info/'):
            return self.getInfo(path[6:])

        if path.startswith('/results'):
            return self.getFile(path[8:])

        # Remove leading slashes
        while path[0] == '/':
            path = path[1:]
        return self.getFile(pjoin(self.pages, path))

        print "Translated", self.path
        self.send_response(400)
        self.end_headers()

    def do_POST(self):
        print "Requested path", self.path
        
        if not self.checkLogin():
            return
        
        # Create FieldStorage from POST request
        form = cgi.FieldStorage(
            fp=self.rfile,
            headers=self.headers,
            environ={'REQUEST_METHOD':'POST',
                     'CONTENT_TYPE':self.headers['Content-Type'],
                     })
        
        # Get requested action
        action = form.getvalue('action', self.path[1:])
        if not action.endswith('.py'):
            action = action + '.py'
        print "Requested", action
        
        # Call the action in a special environment
        output = StringIO()
        tmpout = sys.stdout
        sys.stdout = output
        
        execfile(pjoin(dirname(realpath(__file__)), 'actions', action),
                 dict(form=form, Session=self.Session))
        
        sys.stdout = tmpout
        text = output.getvalue()
        
        self.writeContent(text, 'text/html')


    def Session(self):
        """Session management
        If the client has sent a cookie named sessionId, take its value and 
        return the corresponding SessionElement objet, stored in 
        sessionDict
        Otherwise create a new SessionElement objet and generate a random
        8-letters value sent back to the client as the value for a cookie
        called sessionId"""
        if self.cookie.has_key("sessionId"):
            sessionId = self.cookie["sessionId"].value
        else:
            sessionId = generateRandom(8)
            self.cookie["sessionId"] = sessionId
        try:
            sessionObject = sessionDict[sessionId]
        except KeyError:
            sessionObject = SessionElement()
            sessionDict[sessionId] = sessionObject
        return sessionObject
