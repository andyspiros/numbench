skipUpdate = function(caseID) {
	var skipID, skipmax, skipel, skipli, skipval, nextID, checkboxID, labelID;
	var skipList = document.getElementById('skipList'+caseID);
	
	for (skipID = 0; true; ++skipID) {
		elementID = "skip" + caseID + "_" + skipID;
		skipel = document.getElementById(elementID);
		
		if (skipel == null) {
			skipmax = skipID;
			break;
		}

		skipli = skipel.parentNode;
		
		// Remove void lines
		if (skipel.value.trim() == '')
			skipList.removeChild(skipli);
	}
	
	// Reorder ID's
	nextID = 0;
	for(skipID = 0; skipID < skipmax; ++skipID) {
		elementID = "skip" + caseID + "_" + skipID;
		checkboxID = elementID + "checkbox";
		labelID = elementID + "label";
		
		skipel = document.getElementById(elementID);
		if (skipel == null)
			continue;
		
		elementID = "skip" + caseID + "_" + nextID;
		
		skipel.id = elementID;
		document.getElementById(checkboxID).id = elementID + "checkbox";
		document.getElementById(labelID).id = elementID + "label";
		
		++nextID
	}
	
	// Add a void line at the end
	var newNode = document.createElement('li');
	newNode.appendChild(document.createElement('input'))
	newNode.appendChild(document.createElement('input'))
	newNode.appendChild(document.createElement('label'))
	
	var c = newNode.children;
	
	c[0].value = '';
	c[0].id = "skip" + caseID + "_" + nextID;
	c[0].setAttribute('onchange', 'skipUpdate('+caseID+')');
	
	c[1].setAttribute('type', 'checkbox')
	c[1].id = "skip" + caseID + "_" + nextID + "checkbox";
	
	c[2].id = "skip" + caseID + "_" + nextID + "label";
	c[2].setAttribute('for', c[1].id);
	c[2].innerHTML = 'Regular expression'
	
	skipList.appendChild(newNode);
}
