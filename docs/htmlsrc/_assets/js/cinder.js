var section;
$(document).ready(function() {

	var _this = this;
	var rootDir = window.location.pathname.substr(0, window.location.pathname.lastIndexOf('html/')+5);
	var windowHeight = window.innerHeight;

	var cinderJs = {
		/*
		* ----------------------------------------------------------------------
		*  Selects the correct main nav item
		* ----------------------------------------------------------------------
		*/
		setSection: function( sectionName ){
	 		// find the nav item that correlates to the section name
	 		$("#main-nav").find("li#nav_"+sectionName).addClass( "current" );
	 	},

	 	/*
		* ----------------------------------------------------------------------
		*  Selects the correct namepsace nav iten
		* ----------------------------------------------------------------------
		*/
		selectNamspace: function( namespace ){
	 		var selectedNs = $('#namespace-nav').find('[data-namespace="' + namespace + '"]');
	 		if( selectedNs ){
	 			selectedNs.addClass('active');
	 		}
	 	},

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
	 		var linkTag = $('a[name='+hash+']')[0] || $('a[id='+hash+']');
	 		if( linkTag ) {
	 			linkTag = linkTag[0];
		 		// find parent
		 		var linkParent = $(linkTag.parentNode);
		 		// toggle show for this section
		 		linkParent.removeClass("hidden");
	 		};
 		},

 		/*
		* ----------------------------------------------------------------------
		*  Scroll to anchor on link anchor click
		* ----------------------------------------------------------------------
		*/
 		scrollToAnchor: function(){
 			// return false;
 			// from https://css-tricks.com/snippets/jquery/smooth-scrolling/
 			if( location.pathname.replace(/^\//,'') == this.pathname.replace( /^\//,'' ) && location.hostname == this.hostname ) {
	    		var hash = this.hash;
	    		var target = $( hash );
	      		target = target.length ? target : ($( '[name=' + this.hash.slice(1) +']' ) || $( '[id=' + this.hash.slice(1) +']' ) );

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


	/*
	* --------------------------------------------------------------------------
	*  Side Nav
	* --------------------------------------------------------------------------
	*/
	var sideNav = {
		main: null,
		sections: [],
		init: function(){

			// find all sideNav sections
			this.main = $( '#side-nav' );
			this.sections = this.main.find( 'section' );

			// list depth of side nav
			var baseDepth = this.main.parent('ul').length;

			// Find the depth of each nested list and apply a class based on
			// the depth value. This is used to style the lists.
			_.each( this.sections, function( section ){
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
						if( $li.find( 'ul' ).length > 0 ) {
							$li.addClass( 'list-parent' );
						}
					} );					
				});

				// For each anchor, add magellan arival data
				var anchorLinks = $(section).find( 'a' );
				_.each( anchorLinks, function( aLink ) {
					$aLink = $( aLink );
					var hash = aLink.hash.split('#')[1];
					if( hash ){
						var magArrival = $('<span data-magellan-arrival="' + hash + '"></span>');
						$aLink.wrap( magArrival );
					}

					// if the link is the same as the current page, make it active
					if( aLink.href == window.location.href ) {
						$aLink.parent('li').addClass('active');
					}
				} );
			});

			// for each anchor in the content, wrap the anchor in magellan destination data
			// For each anchor, add magellan arival data
			$( '.content a[href*=#]:not([href=#])' ).click( function() {
		 		cinderJs.scrollToAnchor.apply(this);
		 	});

		 	// find all anchors with name or id
			var anchorTags = $( '.content a[id], .content a[name]' );
			_.each( anchorTags, function( aTag ) {
				$aTag = $( aTag );
				var hash = aTag['name'] || aTag['id'];
				var magDestination = $('<span data-magellan-destination="' + hash + '"></span>');
				$aTag.wrap( magDestination );
			} );

			this.resize();
		},

		resize: function(){
			// change the max side nav height in relation to the window inner height
			if( this.sections.length === 1 ) {
				$( $( this.sections[0] ).find( 'ul' )[0] ).css( "max-height", window.innerHeight - 100 );
			}
		}
	};

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


 	// --- Open all and Close all ---
 	var showAll = function(){
 		$( '.reference-lists .expandable' ).each( function(){
 			var $this = $(this);
 			$this.removeClass("hidden");
 		});
 	};

 	var hideAll = function(){
 		$( '.reference-lists .expandable' ).each( function(){
 			var $this = $(this);
 			$this.addClass("hidden");
 		});
 	};

 	// attach show/hide all functionality
 	var show = $( '#show-hide a.show-all' );
 	var hide = $( '#show-hide a.hide-all' );
 	if( $( '.reference-lists .expandable' ).length > 1 ) {
 		$( '#show-hide' ).removeClass( 'hide' );
 		$( '#show-hide a.show-all' ).on( 'click', showAll );
 		$( '#show-hide a.hide-all' ).on( 'click', hideAll );
 	}
 	

 	// get anchor tag if there is one
 	var hash = window.location.hash.substring(1);

 	// --- Search stuff --- // 
 	window.search = function (term) {
	     
		var resultsDiv = $('#search-results');
	    // the search term must be at least 2 characters
	    if( term.length < 2 ){
	    	resultsDiv.hide();
	    	return;
	    }

	    var maxResults = 15;

	    var results = search_index.search(term); 

		var classResults = $('#search-results-class'),
			guideResults = $('#search-results-guide'),
			nsResults = $('#search-results-namespace'),
			refResults = $('#search-results-reference'),
			moduleResults = $('#search-results-module');

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

		// empty out the uls
		_.each( uls, function(ul) {
			ul.empty();
		});

		if( results.length > 0 ){
			resultsDiv.show();
		} else {
			resultsDiv.hide();
			return false;
		}

		console.log ( 'results', results.length );

		// resultsDiv.append( resultsUl );
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

		// find the max amount per category based on the number of results and the amount of populated categories
		var categoryMax = Math.floor(Math.min(maxResults, results.length) / amtList.length);

		// for each list in the array, only show if they have content
		_.each( uls, function( ul ) {
			var liArr = ul.find('li');
			var listLen = liArr.length;
			if( listLen === 0) {
				ul.parent().hide();
			}else{
				// remove extras
				ul.find('li').slice(categoryMax, listLen).remove();
				ul.parent().show();
			}
		});
	};

	$( '#search-results-view-all' ).on( 'click', function(){
		var searchTerm = escape( document.querySelector('#search-input').value );
		window.location = 'search.html?' + searchTerm;
	} );


	var input = document.querySelector('#search-input');
	if( input ){
		input.addEventListener('input', function()
		{
		    search(input.value);
		});	
	}
	
    
	window.search_index = lunr(function () {
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
	

	// initialization
 	cinderJs.setSection( section );
 	cinderJs.selectNamspace( window.selectedNamespace );
 	cinderJs.showContent( hash );
 	sideNav.init();
 	if( search_index_data )
	 	initSearch();

	 console.log( location )

	// disable history pushState for local files, otherwise, magellan throws an error
	if( location.protocol == "file:" ) {
		history.pushState = false;
	}
	
	// set up magellan stuff
	$(document).foundation({
        "magellan-expedition": {
    //       active_class: 'active', // specify the class used for active sections
          // threshold: 40, // how many pixels until the magellan bar sticks, 0 = auto
          destination_threshold: 35, // pixels from the top of destination for it to be considered active
    //       throttle_delay: 50, // calculation throttling to increase framerate
    //       fixed_top: 0, // top distance in pixels assigend to the fixed element on scroll
          offset_by_height: false // whether to offset the destination by the expedition height. Usually you want this to be true, unless your expedition is on the side.
        }
    });
	$(document).foundation('magellan', 'offcanvas', 'reflow');


	// listen for resize
	$( window ).on( 'resize', function(){
		windowHeight = window.innerHeight;
		sideNav.resize();
		$('#search-results').css('max-height', windowHeight - 100);
	} );
 	return cinderJs;
 } );

window.setSection = function( sectionName ){	
	section = sectionName;
};