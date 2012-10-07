cell2input = function(cell) {
	// Remove event listener
	cell.onclick = null;
	
	var newInput = document.createElement('input');
	newInput.setAttribute('value', cell.innerHTML);
	newInput.setAttribute('onblur', 'input2cell(this)');
	cell.innerHTML = '';
	cell.appendChild(newInput);
	
	// Is varname, then add onTabKey
	if(cell.parentNode.getElementsByTagName('td')[0] == cell)
		cell.onkeydown = onTabKey;
	
	// Give focus
	newInput.focus();
}


input2cell = function(input) {
	// table -> tbody -> tr -> td -> input
	var cell = input.parentNode;
	var row = cell.parentNode;
	var table = row.parentNode.parentNode;
	var caseID = table.id.split('env')[1];
	
	var value = input.value;
	cell.setAttribute('onclick', 'cell2input(this)');
	cell.innerHTML = value;
	
	envUpdate(caseID);
}


onTabKey = function(event) {	
	if (event.keyCode != 9)
		return true;
	
	var input = event.target;	
	var newCell = input.parentNode.parentNode.getElementsByTagName('td')[1];
	cell2input(newCell);
	
	return false;
}


envUpdate = function(caseID) {
	var envs = ['depend', 'emerge', 'compile', 'run'];
	var eid, envname, envtable, envtbody, envtrows, i;
	
	for (eid in envs) {
		envname = envs[eid] + 'env';
		envtable = document.getElementById(envname + caseID);
		
		if (envtable == null)
			continue;
		
		envtbody = envtable.getElementsByTagName('tbody')[0];
		envtrows = envtbody.getElementsByTagName('tr');
		
		// Remove void (i.e. with void variable name) rows
		for (i = 0; i < envtrows.length; ++i) {
			if (envtrows[i].children[0].innerHTML.trim() == '') {
				envtbody.removeChild(envtrows[i]);
				--i;
			}
		}

		// Add void row at the end
		var newRow = document.createElement('tr');
		newRow.appendChild(document.createElement('td'));
		newRow.appendChild(document.createElement('td'));
		
		newRow.children[0].setAttribute('onclick', 'cell2input(this)');
		newRow.children[1].setAttribute('onclick', 'cell2input(this)');
		
		envtbody.appendChild(newRow);
	}
}