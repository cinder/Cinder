var section;
$(document).ready(function() {

	var _this = this;
	var rootDir = window.location.pathname.substr(0, window.location.pathname.lastIndexOf('html/')+5);

	var cinderJs = {
		/*
		* ----------------------------------------------------------------------
 	 	*  Unhides a piece of content if it contains a anchor tag with a 
 	 	*  specific hash
 	 	*  @param {[type]} hash Hash name of the link
 	 	* ----------------------------------------------------------------------
 	 	*/
		showContent: function( hash ){
	 		if( !hash )
	 			return;
	 		// find section with this hash
	 		var linkTag = $('a[name='+hash+']')[0] || $('a[id='+hash+']')[0];
	 		// find parent
	 		var linkParent = $(linkTag.parentNode);
	 		// toggle show for this section
	 		linkParent.removeClass("hidden");
 		},

 		/*
		* ----------------------------------------------------------------------
		*  Scroll to anchor on link anchor click
		* ----------------------------------------------------------------------
		*/
 		scrollToAnchor: function(){
 			// from https://css-tricks.com/snippets/jquery/smooth-scrolling/
 			if( location.pathname.replace(/^\//,'') == this.pathname.replace( /^\//,'' ) && location.hostname == this.hostname ) {
	    		var hash = this.hash;
	    		var target = $( hash );
	      		target = target.length ? target : ($( '[name=' + this.hash.slice(1) +']' ) || $( '[id=' + this.hash.slice(1) +']' ) );
	      		console.log( hash )

	      		if( target.length ) {
	      			var limit = Math.max( document.body.scrollHeight, document.body.offsetHeight, document.documentElement.clientHeight, document.documentElement.scrollHeight, document.documentElement.offsetHeight ) - window.innerHeight; 
	      			var top = Math.min( target.offset().top, limit );
	      			var diff = top - $( this ).scrollTop();
	      			var time = Math.max( Math.min( diff * 0.5, 1200 ), 500 );
	        
			        // animate to anchor
			        $( 'html,body' ).animate( {
			          scrollTop: top,
			          easing: "easeInOutCubic"
			        }, time );

			        // open content at anchor
			        setTimeout(function() {
			        	cinderJs.showContent( hash.split('#')[1] );
			        }, time);
			        return false;
		     	}
			}
	  	} 		
	};


	// Side Nav functions
	var sideNav = {
		main: null,
		sections: [],
		init: function(){
			// find all sideNav sections
			main = $( '#side-nav' );
			sections = main.find( 'section' );
			console.log(main);
			console.log(sections);

			// list depth of side nav
			var baseDepth = main.parent('ul').length;

			// Find the depth of each nested list and apply a class based on
			// the depth value. This is used to style the lists.
			_.each( sections, function( section ){
				var lists = $(section).find( 'ul' );
				_.each(lists, function( list ) {
					// depth of list
					var $list = $( list );
					var listDepth = $list.parents('ul').length - baseDepth;
					$list.addClass( 'depth-' + listDepth );

					// For each list item, add a class if they have nested lists
					var listItems = $list.find( 'li' );
					_.each( listItems, function( li ) {
						$li = $( li );
						// console.log( $li.find( 'ul' ).length );
						if( $li.find( 'ul' ).length > 0 ) {
							$li.addClass( 'list-parent' );
						}
					} );
					
				});


				
			});


		}
	}

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

 		// button.on('click', function(){
 		$this.on('click', function(){
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
 	

 	// get anchor tag if there is one
 	var hash = window.location.hash.substring(1);

 	// --- Search stuff --- // 
 	window.search = function (term) {
	       
	    var results = search_index.search(term); 
		var resultsDiv = $('#search-results');
		var resultsUl = $( "<ul>" );
		var viewAllResults = '<div id="search-results-view-all"><a href="#">View All Results &raquo;</a></div>';

		resultsDiv.empty();
		if( results.length > 0 ){
			resultsDiv.append( resultsUl, viewAllResults );
		}

		// resultsDiv.append( resultsUl );
		for( var i in results ){
			
			var li = $("<li>");
			var result = results[i];
			var data = search_index_data.data[result.ref];
			var link = rootDir + data.link;
			var type = data.type;
			var a = $("<a href=" + link + "> " + data.title + " | " + type + "</a>" );
			li.append(a);
			// li.append(" score: " +  result.score + "</b> ->" );
			// li.append(data.link);
			resultsUl.append( li );
		}

		// hide view all results append when search is empty or no results
		console.log($('#search-results ul'));
		if(!$('#search-input').val() || $('#search-results ul').children().length <= 0) {
			$('#search-results-view-all').hide();
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


	/* 
	* -----------------------------------------------------
	*	Scroll to anchor on link anchor click
	* -----------------------------------------------------
	*/
	$( function() {
	 	$( 'a[href*=#]:not([href=#])' ).click( function() {
	 		cinderJs.scrollToAnchor.apply(this);
	 	});
	} );
	

 	setSection( section );
 	cinderJs.showContent( hash );
 	sideNav.init();
 	if( search_index_data )
	 	initSearch();
 	return cinderJs;
 } );

window.setSection = function( sectionName ){	
	section = sectionName;
};