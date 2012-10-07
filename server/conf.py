username = None
password = None
authMethod = None

def setUserPass(user, passw):
    global username, password, authMethod
    username = user
    password = passw
    
    if password != '':
        if username != '':
            authMethod = 'userpass'
        else:
            authMethod = 'pass'
    else:
        authMethod = 'none'
        
def loginCheck(user='', passw=''):
    if authMethod == 'none':
        return True
    if authMethod == 'pass' and passw == password:
        return True
    if authMethod == 'userpass' and user == username and passw == password:
        return True
    return False