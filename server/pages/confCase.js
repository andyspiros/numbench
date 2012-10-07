
/**
 * Create new void case.
 * 
 * An entry for the case list is returned. The entry is ready to be added to
 * the list and to accept the user's input.
 */
createCase = function(caseID) {
    var main = document.createElement('li');
    main.className = 'case';
    main.id = 'case' + caseID;

    /* Package and ID */
    var idDiv = document.createElement('div');
    idDiv.className = "caseIDdiv";
    idDiv.innerHTML = 
        '<strong>Case ID</strong>: <input id="id' + caseID  + '" />';
    
    var pkgDiv = document.createElement('div');
    pkgDiv.innerHTML = 
        '<strong>Package</strong>: <input id="package' + caseID + '" />'

    main.appendChild(idDiv);
    main.appendChild(pkgDiv);
    
    /* Environments */
    var envs = ['depend', 'emerge', 'compile', 'run'];
    var envText = ['Dependencies ', 'Emerge', 'Compile-time', 'Run-time'];
    
    var basicEnv = document.createElement('div');
    basicEnv.className = 'environmentContainer';
    
    var basicTable = document.createElement('table');
    basicTable.className = 'environment';
    var basicHead = document.createElement('thead');
    basicHead.innerHTML = '<tr><td>Variable name</td><td>Value</td></tr>';
    basicTable.appendChild(basicHead);
    basicTable.innerHTML = basicTable.innerHTML + '<tbody></tbody>';
    
    for (var i = 0; i < 4; ++i) {
        var envTable = basicTable.cloneNode(true);
        envTable.id = envs[i] + "env" + caseID;
        
        var envDiv = basicEnv.cloneNode();
        envDiv.appendChild(envTable);
        envDiv.innerHTML =  '<strong>' + envText[i] +
            ' environment</strong>: ' + envDiv.innerHTML;
        
        main.appendChild(envDiv);
    }
    
    
    /* Skip implementations */
    var skipDiv = document.createElement('div');
    skipDiv.innerHTML = '<strong>Skip implementations</strong>:' + 
        '<ul class="skipList" id="skipList' + caseID + '"></ul>';
    main.appendChild(skipDiv);
    
    
    /* Return */
    return main;
}


/**
 * Create and add a new case to the case list.
 */
addCase = function() {
    var caseLI = createCase(nextCase);
    var button = document.getElementById('addCaseButton');
    document.getElementById('casesList').insertBefore(caseLI, button);
    envUpdate(nextCase);
    skipUpdate(nextCase);
    ++nextCase;
}


/**
 * Return the available module names.
 */
getModuleNames = function() {
    var select = document.getElementById('moduleName');
    var moduleNames = ['blas', 'cblas', 'lapack'];
    
    for (i in moduleNames) {
        option = document.createElement('OPTION');
        option.innerHTML = moduleNames[i];
        select.appendChild(option);
    }
}

/**
 * Initialize the input forms.
 */
initialize = function() {
    nextCase = 0;
    getModuleNames();
    addCase();
}


/**
 * Generate the part of the request string related to the test case caseID
 * and the given table (dependenv, emergeenv,...)
 */
function genReqStrTable(caseID, table) {
    var tRows, varName, varVal;
    var request = '';
    var shortname = table.substr(0, table.length-3);
    
    tRows = document.getElementById(table + caseID).children[1].children;
    for (var i = 0; i < tRows.length; ++i) {
        varName = encodeURIComponent(tRows[i].children[0].innerHTML);
        varVal = encodeURIComponent(tRows[i].children[1].innerHTML);
        if (varName.length > 0)
            request += '&'+shortname+caseID+'_'+varName + '=' + varVal;
    }
    
    return request;
}

/**
 * Generate the part of the request string related to the test case caseID
 */
function genReqStrCase(caseID) {
    var request = '';
    
    
    // ID
    var idIn = document.getElementById('id' + caseID);
    var Cid = encodeURIComponent(idIn.value);
    request += 'id'+caseID + '=' + Cid;
    
    // Package
    var packageIn = document.getElementById('package' + caseID);
    var Cpackage = encodeURIComponent(packageIn.value);
    request += '&package'+caseID + '=' + Cpackage;
    
    // Environments
    request += genReqStrTable(caseID, 'dependenv');
    request += genReqStrTable(caseID, 'emergeenv');
    request += genReqStrTable(caseID, 'compileenv');
    request += genReqStrTable(caseID, 'runenv');
    
    // Skips
    var skipList = document.getElementById('skipList'+caseID).children;
    var skipName, skipRE;
    for (var i = 0; i < skipList.length; ++i) {
        skipName = encodeURIComponent(skipList[i].children[0].value);
        skipRE = skipList[i].children[1].checked ? "1" : "0";
        if (skipName.length > 0)
            request += '&skip'+caseID+'_'+skipName + '=' + skipRE;
    }
    
    return request;
}

/**
 * Generate the request string that describes the whole configuration file.
 */
function generateRequestString() {
    var request = '';
    
    // File name
    var fName = encodeURIComponent(document.getElementById('fName').value);
    if (fName.length > 0)
        request += '&fName='+fName;
    
    // Benchmark title
    var bTit = encodeURIComponent(document.getElementById('benchTitle').value);
    if (bTit.length > 0)
        request += '&benchTitle='+bTit;
    
    // Module name
    var mName = encodeURIComponent(document.getElementById('moduleName').value);
    if (mName.length > 0)
        request += '&moduleName='+mName;
    
    // Module name
    var mArgs = encodeURIComponent(document.getElementById('moduleArgs').value);
    if (mArgs.length > 0)
        request += '&moduleArgs='+mArgs;
    
    // Add cases
    for (var i = 0; i < nextCase-1; ++i)
        request += '&' + genReqStrCase(i);
    
    // Remove leading &
    while(request[0] == '&')
        request = request.substr(1);
    
    return request;
}

