$(document).ready(function() {
	// docsRoot defined in python and passed into master-template.mustache
	var rootDir = window.location.pathname.substr(0, window.location.pathname.lastIndexOf( window.docsRoot + '/' ) + ( window.docsRoot.length + 1 ) );
	var searchTerm = location.search.split("?")[1];

	// --- Search stuff --- // 
 	var fillSearch = function( input ) {
	    var term = unescape( input );
	    var results = search_index.search(unescape(term)); 
		var resultsDiv = $('#page-search-results');
		var resultsUl = $( "<ul>" );
		$('#search-header').html("Search Results for <strong>" + term + "</strong>");

		var classResults = $('#page-search-results-class'),
			guideResults = $('#page-search-results-guide'),
			nsResults = $('#page-search-results-namespace'),
			refResults = $('#page-search-results-reference'),
			moduleResults = $('#page-search-results-module');

		var classResultsUl = classResults.find('ul'),
			guideResultsUl = guideResults.find('ul');
			nsResultsUl = nsResults.find('ul');
			refResultsUl = refResults.find('ul');
			moduleResultsUl = moduleResults.find('ul');
	
		var uls = [classResultsUl, guideResultsUl, nsResultsUl, refResultsUl, moduleResultsUl];
		var amtList = _.range( uls.length ).map(function () { return 0; });
		
		var appendResult = function(ul, li){
			var index = uls.indexOf(ul);
			amtList[index]++;
			ul.append(li);
		};

		for( var i in results ){
			var li = $("<li>");
			var result = results[i];
			var data = search_index_data.data[result.ref];
			var link = rootDir + data.link;
			var type = data.type;
			var a = $("<a href=" + link + "> " + data.title + "</a>" );
			li.append(a);

			switch( type ){
				case 'class':
				case 'struct':
					appendResult( classResultsUl, li );
					break;

				case 'guide':
					appendResult( guideResultsUl, li );
					break;

				case 'namespace':
					appendResult( nsResultsUl, li );
					break;

				case 'reference':
					appendResult( refResultsUl, li );
					break;

				case 'module':
					appendResult( moduleResultsUl, li );
					break;
			}
		}

		// remove empty values
		amtList = _.filter(amtList, function(val){ return val > 0; });

		// for each list in the array, only show if they have content
		_.each( uls, function( ul ) {
			var liArr = ul.find('li');
			var listLen = liArr.length;
			if( listLen === 0) {
				ul.parent().hide();
			}else{
				ul.parent().show();
			}
		});
	};

	fillSearch( searchTerm );
});