#@PydevCodeAnalysisIgnore
import conf

# Define pages
page = '''\
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
        "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
 < html xmlns = "http://www.w3.org/1999/xhtml" lang = "en" xml:lang = "en" >
<head>
<title>Numbench: login</title>
<meta http-equiv="refresh" content="%d;url=%s">
</head>
<body>
%s
</body>
</html>
'''

sd = Session().d

success = conf.loginCheck(form.getvalue('username'), form.getvalue('password'))

if success:
    redirecturl = sd.get('requested', '/index.html')
    sd['login'] = form.getvalue('username')
    redirecttime = 3
    message = 'Login succeded. You will be redirected shortly. If this does' \
              'not happen, use the following link: ' \
              '<a href="%s">%s</a>.' % (2 * (redirecturl,))

else:
    redirecturl = '/login.html'
    redirecttime = 0
    message = 'Login failure. You will be redirected shortly. If this does' \
              'not happen, use the following link: ' \
              '<a href="%s">%s</a>.' % (2 * (redirecturl,))

print page % (redirecttime, redirecturl, message)
