var section;
$(document).ready(function() {

	var cinderJs = this;
	
	// look for all dom items with class

 	// find all expandable sections
 	var expandableSections = $( '.expandable' );
 	expandableSections.each( function() {

 		var $this = $(this);
 		var content = $( $this.find('.content')[0] );
 		var maxHeight = content.innerHeight();

 		// set height to max height
 		content.css('height', maxHeight + 'px');

 		// add a button
 		var button = $('<button class="expand"></button>');
 		$this.prepend( button );

 		button.on('click', function(){
 			$this.toggleClass('hidden');
 		}); 		
 		
 		// Hide all of the definition tags
 		if( $this.find( ".functionDef, .enumDef, .definitionCol" ).length > 0 ){
 			$this.addClass("hidden");
 		}
 	});

 	var setSection = function( sectionName ){
 		// find the nav item that correlates to the section name
 		// console.log( "CUR SECTION", sectionName );
 		// console.log( $("#main-nav").find("li#nav_"+sectionName) );
 		$("#main-nav").find("li#nav_"+sectionName).addClass( "current" );
 	};

 	// --- Open all and Close all ---
 	var showAll = function(){
 		$( '.contents .expandable' ).each( function(){
 			var $this = $(this);
 			$this.removeClass("hidden");
 		});
 	};

 	var hideAll = function(){
 		$( '.contents .expandable' ).each( function(){
 			var $this = $(this);
 			$this.addClass("hidden");
 		});
 	};

 	// attach show/hide all functionality
 	$( '#show-hide a.show-all' ).on( 'click', showAll );
 	$( '#show-hide a.hide-all' ).on( 'click', hideAll );
 	

 	/**
 	 * Unhides a piece of content if it contains a anchor tag with a specific hash
 	 * @param {[type]} hash Hash name of the link
 	 */
 	var showContent = function(hash){
 		if( !hash )
 			return;
 		// find section with this hash
 		var linkTag = $('a[name='+hash+']')[0];
 		// find parent
 		var linkParent = $(linkTag.parentNode);
 		// toggle show for this section
 		linkParent.removeClass("hidden");
 	};
 	
 	// get anchor tag if there is one
 	var hash = window.location.hash.substring(1);
 	

 	// --- Search stuff --- // 
 	window.search = function (term) {
	       
	    var results = search_index.search(term); 
		var resultsDiv = $('#search-results');
		var resultsUl = $( "<ul>" );

		resultsDiv.empty();
		resultsDiv.append( resultsUl );

		// resultsDiv.append( resultsUl );
		for( var i in results ){
			
			var li = $("<li>");
			var result = results[i];
			var data = search_index_data.data[result.ref];
			var a = $("<a href=" + data.link + "> " +  data.title +"</a>");
			li.append(a);
			li.append(" score: " +  result.score + "</b> ->" );
			
			li.append(data.link);
			resultsUl.append( li );
		}
	};

	var input = document.querySelector('#search-input');
	if( input ){
		input.addEventListener('input', function()
		{
		    search(input.value);
		});	
	}
	
    
	var search_index = lunr(function () {
		this.field('title', {boost: 10});
		this.field('tags', {boost: 1});
		this.field('body');
		this.ref('id');
	});

	function initSearch(){
		
		for(var item in search_index_data.data){
			var searchItem = search_index_data.data[item];
			// console.log(searchItem.tags);
			search_index.add({
			    id: searchItem.id,
			    title: searchItem.title,
			    body: searchItem.body,
			    tags: searchItem.tags
			 });
		}
	}
	

 	setSection( section );
 	showContent( hash );
 	if( search_index_data )
	 	initSearch();
 	return cinderJs;

 } );

window.setSection = function( sectionName ){	
	section = sectionName;
};
