import signal, sys, argparse
from Server import MyRequestHandler
from BaseHTTPServer import HTTPServer
import conf as cfg

# Set signal handler
def sighandler(sig, frame):
    print "Signal", sig, "triggered"
    sys.exit(0)

signal.signal(signal.SIGINT, sighandler)
signal.signal(signal.SIGTERM, sighandler)

# Argument parser
argparser = argparse.ArgumentParser()
argparser.add_argument('-p', '--port', help="Port at which the server listens",
                       type = int, default = 8123)
argparser.add_argument('-u', '--username', default='',
                       help = "User name used for the authentication")
argparser.add_argument('-s', '--secret', default='',
                       help = "Password used for the authentication")
args = argparser.parse_args()


# Store argument values
cfg.setUserPass(args.username, args.secret)

port = args.port
print "Listening at port", port

# Start the webserver
httpd = HTTPServer(('', port), MyRequestHandler)
httpd.serve_forever()
