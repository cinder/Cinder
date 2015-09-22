$(document).ready(function() {
	var rootDir = window.location.pathname.substr(0, window.location.pathname.lastIndexOf('html/')+5);
	var searchTerm = location.search.split("?")[1];

	// --- Search stuff --- // 
 	var fillSearch = function (term) {
	       
	    var results = search_index.search(term); 
		var resultsDiv = $('#page-search-results');
		var resultsUl = $( "<ul>" );
		$('#search-header').html("Search Results for <strong>" + term + "</strong>");
	
		resultsDiv.empty();
		if( results.length > 0 ){
			resultsDiv.append( resultsUl );
		}

		for( var i in results ){
			
			var li = $("<li>");
			var result = results[i];
			var data = search_index_data.data[result.ref];
			var link = rootDir + data.link;
			var type = data.type;
			var a = $("<a href=" + link + "> " + data.title + " | " + type + "</a>" );
			li.append(a);
			resultsUl.append( li );
		}
	};

	fillSearch( searchTerm );
});