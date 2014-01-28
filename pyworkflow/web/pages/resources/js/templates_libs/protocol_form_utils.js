 /*****************************************************************************
 *
 * Authors:    Jose Gutierrez (jose.gutierrez@cnb.csic.es)
 * 			   Adrian Quintana (aquintana@cnb.csic.es)
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'jmdelarosa@cnb.csic.es'
 *
 ******************************************************************************/
/******************************************************************************
 * DESCRIPTION:
 * 
 * Methods to manage the protocol form.
 * 
 * ATTRIBUTES LIST:
 * 
 * METHODS LIST:
 * 
 * jQuery(document).ready(function())
 * 	->	Overray the post simple method in the protocol form template. 
 * 		Depend a variable of the protocol form, can be:
 * 		* Execute protocol: This mode execute the protocol with the parameters of
 * 			the form.
 * 		* Save protocol: This method save the protocol with the parameters filled.
 * 		* Wizard: This mode launch a wizard for a specific parameter.   
 * 		* Viewer: This mode launch a viewer to analyze the results for the parameters
 * 			chosen in the form.
 * 
 * function evalElements()
 * 	->	Function to evaluate the elements in a form, depending the type of the
 * 		param, it is evaluated in a diferent way.
 * 
 * function onChangeParam(value, paramId)
 * 	->	Update the parameter for an element.
 * 
 * function onChangeEnumParamCombo(elemId, paramId)
 * 	->	Update the parameter for an element type ENUM_PARAM_COMBO.
 * 
 * function onChangeEnumParamList(index, paramId)
 * 	->	Update the parameter for an element type ENUM_PARAM_LIST.
 * 
 * function setParamValue(paramId, value)
 * 	->	Put the new value in an attribute of the parent node.
 * 
 * function evalDependencies(row, newLevel)
 * 	->	Function to evaluate the parameters dependencies for a expertise level.
 * 
 * function evalCondition(row)
 * 	->	Function to evaluate a condition given a row of the form.
 * 
 * function normalizeConditions(cond)
 * 	->	For some conditions not normalized, it is replaced to be evaluated right.
 * 
 * function browseObjects(param, projName, type_param, param)
 * 	->	Browse object in the database depending of a type_param. 
 * 		Params: 
 * 			objClass: the class to get instances from (also subclasses)
 * 			protClassName: class refered to a protocol
 * 
 * function formProtSimple(param, projName)
 * 	->	Launch a custom protocol form with less options, thought for workflows
 * 		where some options not need to be chosen.
 * 
 * function returnProtocol()
 * 	->	Function to return a simple protocol form with the values serializes to be
 * 		saved, used for the custom workflow where the parameters need to be changed.
 * 
 * function setParamProt(paramProt, params)
 * 	->	Function to update a protocol serialize param inside the workflow.
 * 		The serializes values are storaged inside the input respectively.
 * 
 * function getTableFormatted(node, list, id, previsualize)
 * 	->	This function return an html table with the elements available after use, 
 * 		the function browse_object.
 * 
 * function selectDialog(objClass, msg, funcName)
 * 	->	This function create a popup with messi.js to choose the elements showed
 * 		with the method getTableFormatted.
 * 
 * function processSelectionTable(elm)
 * 	->	After select an element showed with the function selectDialog with the 
 * 		table generated by the function getTableFormatted, this selection is 
 * 		processed with this method.
 * 
 * function selTableMessi(elm)
 * 	->	Function to select the element dinamically with flashlighted colors in 
 * 		the table generated with the function getTableFormatted.
 * 
 ******************************************************************************/

/** METHODS ******************************************************************/

$(document).ready(function() {
	/*	
	* Overray the post simple method in the protocol form template. 
	* Depend a variable of the protocol form 
	*/
	$("#protocolForm").submit(function() {
		var mode = $("#protocolForm").attr('data-mode');

		if (mode == 'execute') {
			/* Execute the protocol */
			var action = $("#protocolForm").attr("action");
			
			console.log($("#protocolForm").serialize())
			
			var serialize_form = fixInput($("#protocolForm").serialize());
			
			console.log(serialize_form) 

			$.post(action, serialize_form, function(json) {
				if (json.errors.length > 0) {
					// Show errors in the validation
					errorPopup('Errors found',json.errors);
				} else {
					infoPopup('Success', "The protocol was launched successfuly",1);
				}
			}, "json");

		} else if (mode == 'save') {
			/* Save the protocol */
			var action = "/save_protocol/";
			
			var serialize_form = fixInput($("#protocolForm").serialize());
			
			$.post(action, serialize_form, function(json) {
				if (json.errors != undefined) {
					// Show errors in the process to save
					errorPopup('Errors found',json.errors);
				} else {
					// No errors in the process to save
					protId = json.success;
					infoPopup('Success', "The protocol was saved successfuly",1,'window.opener.popup(\'/form/?protocolId='+protId+'\')');
					
				}
			},"json");
		} else if (mode == 'wiz') {
			
			new Messi("<i class='fa fa-magic'/>  Loading Wizard...",{
				modal : true
				});
			
			/* Execute the wizard */
			var action = "/wizard/";
			var type_wiz = $("#wizName").attr("value");
			
			$.post(action, $("#protocolForm").serialize(), function(html) {
				
				$('.messi').remove();
				$('.messi-modal').remove();
				
				if(html=="errorInput"){
					errorPopup("Error", "Input was not selected, please choose one.");
				} else if (html=="errorEmpty"){
					errorPopup("Error", "Input particles selected are None");
				} else if (html=="errorIterate"){
					errorPopup("Error", "Error iterating over the set of particles");
				} else if(type_wiz=='wiz_particle_mask' || type_wiz=='wiz_volume_mask'){
					customPopupHTML(html,540,490);
				} else if(type_wiz=='wiz_volume_mask_radii' || type_wiz=='wiz_particle_mask_radii'){
					customPopupHTML(html,550,540);
				} else{
					customPopupHTML(html,790,480);
				}
			});
		} else if (mode == 'viewer' || mode == 'viewerElement') {
			
			new Messi("<i class='fa fa-eye'/> Loading Viewer...",{
				modal : true
				});
			
			/* Launch the viewers with the options chosen */
			var action = "/"+ mode +"/";

			$.post(action, $("#protocolForm").serialize(), function(json) {
				$('.messi').remove();
				$('.messi-modal').remove();				
				popUpJSON(json);
			},"json");			
		} 
		// Important. Stop the normal POST
		return false;
	});
});

function fixInput(serialize_form){
	var attrs = serialize_form.split("&");

	$.each(attrs, function(param, paramName) {
		console.log(param)
		console.log(paramName)
		var aux = paramName.split("=");
		if($("#"+aux[0]+"_input")){
			var objId = $("#"+aux[0]+"_input").attr("data-objId");
			if (objId){
				console.log("eybaby")
				console.log(paramName)
				serialize_form = serialize_form.replace(paramName , aux[0]+"="+objId)
			}
		}
	});
	return serialize_form
}


function evalElements() {
	/*
	 * Function to evaluate the elements in a form, depending the type of the
	 * param, it is evaluated in a diferent way 
	 */
	
	$("tr").each(function(index) {
//		
		var value = jQuery(this).val();
		if(value.length == 0){
			var value = jQuery(this).attr('value');
		}
		var type = jQuery(this).attr('data-type');
		var param = jQuery(this).attr('id');

//		alert(value +" - "+type+" - "+param);

//		if (type == "BooleanParam" || type == "FloatParam" || type == "IntParam") {
//			onChangeBooleanParam(value, param);
//		} else 
		if (type == "EnumParam") {
			var typeEnum = jQuery(this).attr('data-enum');
			if (typeEnum == '0') {
				onChangeEnumParamList(value, param);
			} else if (typeEnum == '1') {
				onChangeEnumParamCombo(param + "_select", param);
			}
		} else {
//			alert("before:"+value);
			onChangeParam(value, param);
		}
	});
}

function onChangeParam(value, paramId) {
	/* 
	 * Update the parameter for an element.
	 */
	
//	alert(paramId + "-"+value);
	setParamValue(paramId, value);
}
	

function onChangeEnumParamCombo(elemId, paramId) {
	/*
	 * Update the parameter for an element type ENUM_PARAM_COMBO.
	 */
	var elem = document.getElementById(elemId);
	setParamValue(paramId, elem.selectedIndex);
}

function onChangeEnumParamList(index, paramId) {
	/*
	 * Update the parameter for an element type ENUM_PARAM_LIST.
	 */
	setParamValue(paramId, index);
}

function setParamValue(paramId, value) {
	/*
	 * Put the new value in an attribute of the parent node.
	 */
	var row = jQuery("tr#" + paramId);
	//	alert("before:"+row.val());
	row.val(value);	
	//	alert("after:"+row.val());
	
	var newLevel = $("select[name=expertLevel]").val();
	evalDependencies(row, newLevel);

	var params = row.attr('data-params');

	if (params != undefined && params.length <= 0) {
					
		var expLevel = row.attr('data-expert');
	
		if (expLevel > newLevel) {
			row.hide();
		} else {
			row.show();			
		}
	}
}

function evalDependencies(row, newLevel) {
	/*
	 * Function to evaluate the parameters dependencies for a expertise level.
	 */
	
	//	var newLevel = $("select[name=expLevel]").val();
	var dependencies = row.attr('data-depen');
	if (dependencies!= undefined && dependencies.length > 0) {
		var arrayDepends = dependencies.split(",");
		for ( var cont = 0; cont < arrayDepends.length; cont++) {

			var row2 = jQuery("tr#" + arrayDepends[cont]);
			var res = evalCondition(row2);
			var expLevel = row2.attr('data-expert');
			
//			alert("level:"+expLevel+", newlevel:"+newLevel)

			if (res == false || expLevel > newLevel) {
				row2.hide();
			} else if (res == true) {
				row2.show();
				evalDependencies(row2, newLevel);
			}
		}
	}
}

function evalCondition(row) {
	/*
	 * Function to evaluate a condition given a row of the form.
	 */

	var cond = row.attr('data-cond');
	var params = row.attr('data-params');

	var arrayParams = params.split(",");

	// Get value of the element with name=itenName
	var param = null;
	var value = null;
	var cond_eval = cond;
	// var params = '';

	for ( var cont = 0; cont < arrayParams.length; cont++) {
		param = arrayParams[cont];
		// value = getValueByName(param);
		value = jQuery("tr#" + param).val();
		if (!value){
			value = jQuery("tr#" + param).attr("value");
			if (!value){
				value="''";
			}
		}
//		params += "param: " + param + " value: " + value + "\n";
		cond_eval = cond_eval.replace(param, value);
	}
	
//	if (row.attr("name")=="comment") {
//		alert("condition: " + cond + " \nparams:\n" + params + "\n eval: " + cond_eval);
//	}
	
	cond_eval = normalizeConditions(cond_eval)

//	var foundAnd = cond.indexOf("'0'") != -1;
//	if (foundAnd)
//		alert("condition: " + cond + " \neval: " + cond_eval+ " \nparams:"+ params );

	return eval(cond_eval);
}

function normalizeConditions(cond){
	/*
	 * For some conditions not normalized, it is replaced to be evaluated right.
	 */
	cond = cond.replace("not","!");
	cond = cond.replace("and","&&");
	cond = cond.replace("or","||");
	cond = cond.replace("'0'","false");
	cond = cond.replace("'1'","true");
	return cond;
}

function browseObjects(param, projName, type_param, value_param, pointerCondition, maxNumObjects) {
	/*
	 * Browse object in the database. 
	 * Params: objClass: the class to get instances from (also subclasses)
	 * protClassName: class refered to a protocol
	 */
	
	var url_param = "/browse_objects/?projectName=" + projName + "&objClass=" + value_param + "&objFilter=" + pointerCondition
	if (type_param == 'protClassName'){
		url_param = "/browse_protocol_class/?projectName=" + projName + "&protClassName=" + value_param
	}
		
	$.ajax({
		type : "GET",
		url : url_param,
		dataType : "json",
		success : function(json) {
			// specifying a dataType of json makes jQuery pre-eval the response
			// for us
			var res = getTableFormatted(param, json, value_param, 1);
			var selectionFunc = "processSelectionTable"
			if (maxNumObjects == 0 || maxNumObjects >1){
				selectionFunc = "processMultipleSelectionTable"
			}
			selectDialog(param, res, selectionFunc);
		}
	});
}

function formProtSimple(param, projName){
	/*
	 * Launch a custom protocol form with less options, thought for workflows
	 * where some options not need to be chosen.
	 */
	var protSimple = $("#"+param +"_input").val();
	var dataProt = $("#"+param+"_input").attr("data-prot")
	
	if (protSimple.length > 0){
		if(dataProt != undefined){
			// load the protocol params in the form
			var url = '/form/?protocolClass='+protSimple+'&action=protSimple&paramProt='+param+'&'+dataProt
		} else {
			// load a blank form with a new protocol
			var url = '/form/?protocolClass='+protSimple+'&action=protSimple&paramProt='+param
		}
		customPopup(url,500,450);
	}
	else{
		errorPopup("Error", "Protocol was not selected, please choose one.")
	}
}

function returnProtocol(){
	/*
	 * Function to return a simple protocol form with the values serializes to be
	 * saved, used for the custom workflow where the parameters need to be changed.
	 */
	params = $("#protocolForm").serialize();
	paramProt = $("#paramProt").val();
	window.opener.setParamProt(paramProt, params);
	infoPopup("Successful", "Protocol saved inside the workflow", 1);
}

function setParamProt(paramProt, params){
	/*
	 * Function to update a protocol serialize param inside the workflow.
	 * The serializes values are storaged inside the input respectively.
	 */
	$("#"+paramProt+"_input").attr("data-prot", params)
}

//function getListFormatted(node, list, id) {
//	var res = "<div class='content' style='overflow:auto' data-node='" + node
//			+ "'>";
//	for ( var x = 0; x < list.length; x++) {
//		res += "<input type='radio' id ='" + id + x + "' name='" + id
//				+ "'  value='" + list[x] + "' />" + list[x] + "<br />";
//	}
//	res = res + "</div>";
//	return res;
//}

function getTableFormatted(node, json, id, previsualize) {
	/*
	 * This function return an html table with the elements available after use, 
	 * the function browse_object.
	 */
	var res = "<table class='content' style='overflow:auto' data-node='" + node
			+ "'>";
	
	var func = "";
	var first = "<a class='fa fa-eye' href='javascript:";
	var second = "'></a>";
	
	var x = 0;
	$.each(json, function(key, value) {
		// key is the param ObjId for the object
		// value is the name of the object
		if(previsualize){
			var func = "&nbsp&nbsp&nbsp" + first + 'customPopup("/visualize_object/?objectId='+ key +'",1024,600)' + second;
		}
				
		res += "<tr><td id='" + id + x + "' class='" + key + "' value='"
				+ value + "' onclick=javascript:selTableMessi($(this)); >" 
				+ value + func +"</td></tr>";
		x++;
	});
	
	res = res + "</table>";
	return res;
}

function selectDialog(objClass, msg, funcName) {
	/*
	 * This function create a popup with messi.js to choose the elements showed
	 * with the method getTableFormatted. 
	 */
	new Messi(msg, {
		title : 'Select ' + objClass,
		modal : true,
		buttons : [ {
			id : 0,
			label : 'Select',
			val : 'Y',
			btnClass : 'fa-check',
			btnFunc : funcName
		}, {
			id : 1,
			label : 'Cancel',
			val : 'C',
			btnClass : 'fa-ban'
		} ]
	});
}

function getSelectedValue(elm){
	var res = []
	           
	var selected = $("td#" + elm.attr('value'));
	res[0] = selected.attr('value')
	res[1] = selected.attr('class');
	
	return res;
}

function processSelectionTable(elm) {
	/*
	 * After select an element showed with the function selectDialog with the 
	 * table generated by the function getTableFormatted, this selection is 
	 * storaged with this method.
	 */
	var value = getSelectedValue(elm);
//	$('input#' + elm.attr('data-node') + '_input').val(value[0]);
	$('input#' + elm.attr('data-node') + '_input').attr('value', value[0]); 
	$('input#' + elm.attr('data-node') + '_input').attr('data-objId', value[1]); 
}

function processMultipleSelectionTable(elm) {
	/*
	 * After select an element showed with the function selectDialog with the 
	 * table generated by the function getTableFormatted, this selection is 
	 * storaged with this method.
	 */
	var value = getSelectedValue(elm);
	var exists = false;
	$('#'+elm.attr('data-node') + '_input option').each(function(){
		if (this.value == value[1]){
			exists = true;
			return false;
		}
	});
	
	if (exists){
		errorPopup("Selection Error","File already selected")
	}
	else{
		$('#'+elm.attr('data-node') + '_input').append('<option value='+value[1]+'>'+value[0]+'</option>')
	}	
	
}

function selTableMessi(elm) {
/*
 * Used to choose a element in the protocol form
 */
	var row = $("table.content");
	var id = elm.attr('id');

	if (row.attr('value') != undefined && row.attr('value') != id) {
		var rowOld = $("td#" + row.attr('value'));
		rowOld.attr('style', '');
	}
	row.attr('value', id);
	elm.attr('style', 'background-color: #eaebff;');
}

