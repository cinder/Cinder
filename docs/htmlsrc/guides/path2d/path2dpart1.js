/*! jQuery v1.10.1 | (c) 2005, 2013 jQuery Foundation, Inc. | jquery.org/license
//@ sourceMappingURL=jquery-1.10.1.min.map
*/
(function(e,t){var n,r,i=typeof t,o=e.location,a=e.document,s=a.documentElement,l=e.jQuery,u=e.$,c={},p=[],f="1.10.1",d=p.concat,h=p.push,g=p.slice,m=p.indexOf,y=c.toString,v=c.hasOwnProperty,b=f.trim,x=function(e,t){return new x.fn.init(e,t,r)},w=/[+-]?(?:\d*\.|)\d+(?:[eE][+-]?\d+|)/.source,T=/\S+/g,C=/^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g,N=/^(?:\s*(<[\w\W]+>)[^>]*|#([\w-]*))$/,k=/^<(\w+)\s*\/?>(?:<\/\1>|)$/,E=/^[\],:{}\s]*$/,S=/(?:^|:|,)(?:\s*\[)+/g,A=/\\(?:["\\\/bfnrt]|u[\da-fA-F]{4})/g,j=/"[^"\\\r\n]*"|true|false|null|-?(?:\d+\.|)\d+(?:[eE][+-]?\d+|)/g,D=/^-ms-/,L=/-([\da-z])/gi,H=function(e,t){return t.toUpperCase()},q=function(e){(a.addEventListener||"load"===e.type||"complete"===a.readyState)&&(_(),x.ready())},_=function(){a.addEventListener?(a.removeEventListener("DOMContentLoaded",q,!1),e.removeEventListener("load",q,!1)):(a.detachEvent("onreadystatechange",q),e.detachEvent("onload",q))};x.fn=x.prototype={jquery:f,constructor:x,init:function(e,n,r){var i,o;if(!e)return this;if("string"==typeof e){if(i="<"===e.charAt(0)&&">"===e.charAt(e.length-1)&&e.length>=3?[null,e,null]:N.exec(e),!i||!i[1]&&n)return!n||n.jquery?(n||r).find(e):this.constructor(n).find(e);if(i[1]){if(n=n instanceof x?n[0]:n,x.merge(this,x.parseHTML(i[1],n&&n.nodeType?n.ownerDocument||n:a,!0)),k.test(i[1])&&x.isPlainObject(n))for(i in n)x.isFunction(this[i])?this[i](n[i]):this.attr(i,n[i]);return this}if(o=a.getElementById(i[2]),o&&o.parentNode){if(o.id!==i[2])return r.find(e);this.length=1,this[0]=o}return this.context=a,this.selector=e,this}return e.nodeType?(this.context=this[0]=e,this.length=1,this):x.isFunction(e)?r.ready(e):(e.selector!==t&&(this.selector=e.selector,this.context=e.context),x.makeArray(e,this))},selector:"",length:0,toArray:function(){return g.call(this)},get:function(e){return null==e?this.toArray():0>e?this[this.length+e]:this[e]},pushStack:function(e){var t=x.merge(this.constructor(),e);return t.prevObject=this,t.context=this.context,t},each:function(e,t){return x.each(this,e,t)},ready:function(e){return x.ready.promise().done(e),this},slice:function(){return this.pushStack(g.apply(this,arguments))},first:function(){return this.eq(0)},last:function(){return this.eq(-1)},eq:function(e){var t=this.length,n=+e+(0>e?t:0);return this.pushStack(n>=0&&t>n?[this[n]]:[])},map:function(e){return this.pushStack(x.map(this,function(t,n){return e.call(t,n,t)}))},end:function(){return this.prevObject||this.constructor(null)},push:h,sort:[].sort,splice:[].splice},x.fn.init.prototype=x.fn,x.extend=x.fn.extend=function(){var e,n,r,i,o,a,s=arguments[0]||{},l=1,u=arguments.length,c=!1;for("boolean"==typeof s&&(c=s,s=arguments[1]||{},l=2),"object"==typeof s||x.isFunction(s)||(s={}),u===l&&(s=this,--l);u>l;l++)if(null!=(o=arguments[l]))for(i in o)e=s[i],r=o[i],s!==r&&(c&&r&&(x.isPlainObject(r)||(n=x.isArray(r)))?(n?(n=!1,a=e&&x.isArray(e)?e:[]):a=e&&x.isPlainObject(e)?e:{},s[i]=x.extend(c,a,r)):r!==t&&(s[i]=r));return s},x.extend({expando:"jQuery"+(f+Math.random()).replace(/\D/g,""),noConflict:function(t){return e.$===x&&(e.$=u),t&&e.jQuery===x&&(e.jQuery=l),x},isReady:!1,readyWait:1,holdReady:function(e){e?x.readyWait++:x.ready(!0)},ready:function(e){if(e===!0?!--x.readyWait:!x.isReady){if(!a.body)return setTimeout(x.ready);x.isReady=!0,e!==!0&&--x.readyWait>0||(n.resolveWith(a,[x]),x.fn.trigger&&x(a).trigger("ready").off("ready"))}},isFunction:function(e){return"function"===x.type(e)},isArray:Array.isArray||function(e){return"array"===x.type(e)},isWindow:function(e){return null!=e&&e==e.window},isNumeric:function(e){return!isNaN(parseFloat(e))&&isFinite(e)},type:function(e){return null==e?e+"":"object"==typeof e||"function"==typeof e?c[y.call(e)]||"object":typeof e},isPlainObject:function(e){var n;if(!e||"object"!==x.type(e)||e.nodeType||x.isWindow(e))return!1;try{if(e.constructor&&!v.call(e,"constructor")&&!v.call(e.constructor.prototype,"isPrototypeOf"))return!1}catch(r){return!1}if(x.support.ownLast)for(n in e)return v.call(e,n);for(n in e);return n===t||v.call(e,n)},isEmptyObject:function(e){var t;for(t in e)return!1;return!0},error:function(e){throw Error(e)},parseHTML:function(e,t,n){if(!e||"string"!=typeof e)return null;"boolean"==typeof t&&(n=t,t=!1),t=t||a;var r=k.exec(e),i=!n&&[];return r?[t.createElement(r[1])]:(r=x.buildFragment([e],t,i),i&&x(i).remove(),x.merge([],r.childNodes))},parseJSON:function(n){return e.JSON&&e.JSON.parse?e.JSON.parse(n):null===n?n:"string"==typeof n&&(n=x.trim(n),n&&E.test(n.replace(A,"@").replace(j,"]").replace(S,"")))?Function("return "+n)():(x.error("Invalid JSON: "+n),t)},parseXML:function(n){var r,i;if(!n||"string"!=typeof n)return null;try{e.DOMParser?(i=new DOMParser,r=i.parseFromString(n,"text/xml")):(r=new ActiveXObject("Microsoft.XMLDOM"),r.async="false",r.loadXML(n))}catch(o){r=t}return r&&r.documentElement&&!r.getElementsByTagName("parsererror").length||x.error("Invalid XML: "+n),r},noop:function(){},globalEval:function(t){t&&x.trim(t)&&(e.execScript||function(t){e.eval.call(e,t)})(t)},camelCase:function(e){return e.replace(D,"ms-").replace(L,H)},nodeName:function(e,t){return e.nodeName&&e.nodeName.toLowerCase()===t.toLowerCase()},each:function(e,t,n){var r,i=0,o=e.length,a=M(e);if(n){if(a){for(;o>i;i++)if(r=t.apply(e[i],n),r===!1)break}else for(i in e)if(r=t.apply(e[i],n),r===!1)break}else if(a){for(;o>i;i++)if(r=t.call(e[i],i,e[i]),r===!1)break}else for(i in e)if(r=t.call(e[i],i,e[i]),r===!1)break;return e},trim:b&&!b.call("\ufeff\u00a0")?function(e){return null==e?"":b.call(e)}:function(e){return null==e?"":(e+"").replace(C,"")},makeArray:function(e,t){var n=t||[];return null!=e&&(M(Object(e))?x.merge(n,"string"==typeof e?[e]:e):h.call(n,e)),n},inArray:function(e,t,n){var r;if(t){if(m)return m.call(t,e,n);for(r=t.length,n=n?0>n?Math.max(0,r+n):n:0;r>n;n++)if(n in t&&t[n]===e)return n}return-1},merge:function(e,n){var r=n.length,i=e.length,o=0;if("number"==typeof r)for(;r>o;o++)e[i++]=n[o];else while(n[o]!==t)e[i++]=n[o++];return e.length=i,e},grep:function(e,t,n){var r,i=[],o=0,a=e.length;for(n=!!n;a>o;o++)r=!!t(e[o],o),n!==r&&i.push(e[o]);return i},map:function(e,t,n){var r,i=0,o=e.length,a=M(e),s=[];if(a)for(;o>i;i++)r=t(e[i],i,n),null!=r&&(s[s.length]=r);else for(i in e)r=t(e[i],i,n),null!=r&&(s[s.length]=r);return d.apply([],s)},guid:1,proxy:function(e,n){var r,i,o;return"string"==typeof n&&(o=e[n],n=e,e=o),x.isFunction(e)?(r=g.call(arguments,2),i=function(){return e.apply(n||this,r.concat(g.call(arguments)))},i.guid=e.guid=e.guid||x.guid++,i):t},access:function(e,n,r,i,o,a,s){var l=0,u=e.length,c=null==r;if("object"===x.type(r)){o=!0;for(l in r)x.access(e,n,l,r[l],!0,a,s)}else if(i!==t&&(o=!0,x.isFunction(i)||(s=!0),c&&(s?(n.call(e,i),n=null):(c=n,n=function(e,t,n){return c.call(x(e),n)})),n))for(;u>l;l++)n(e[l],r,s?i:i.call(e[l],l,n(e[l],r)));return o?e:c?n.call(e):u?n(e[0],r):a},now:function(){return(new Date).getTime()},swap:function(e,t,n,r){var i,o,a={};for(o in t)a[o]=e.style[o],e.style[o]=t[o];i=n.apply(e,r||[]);for(o in t)e.style[o]=a[o];return i}}),x.ready.promise=function(t){if(!n)if(n=x.Deferred(),"complete"===a.readyState)setTimeout(x.ready);else if(a.addEventListener)a.addEventListener("DOMContentLoaded",q,!1),e.addEventListener("load",q,!1);else{a.attachEvent("onreadystatechange",q),e.attachEvent("onload",q);var r=!1;try{r=null==e.frameElement&&a.documentElement}catch(i){}r&&r.doScroll&&function o(){if(!x.isReady){try{r.doScroll("left")}catch(e){return setTimeout(o,50)}_(),x.ready()}}()}return n.promise(t)},x.each("Boolean Number String Function Array Date RegExp Object Error".split(" "),function(e,t){c["[object "+t+"]"]=t.toLowerCase()});function M(e){var t=e.length,n=x.type(e);return x.isWindow(e)?!1:1===e.nodeType&&t?!0:"array"===n||"function"!==n&&(0===t||"number"==typeof t&&t>0&&t-1 in e)}r=x(a),function(e,t){var n,r,i,o,a,s,l,u,c,p,f,d,h,g,m,y,v,b="sizzle"+-new Date,w=e.document,T=0,C=0,N=lt(),k=lt(),E=lt(),S=!1,A=function(){return 0},j=typeof t,D=1<<31,L={}.hasOwnProperty,H=[],q=H.pop,_=H.push,M=H.push,O=H.slice,F=H.indexOf||function(e){var t=0,n=this.length;for(;n>t;t++)if(this[t]===e)return t;return-1},B="checked|selected|async|autofocus|autoplay|controls|defer|disabled|hidden|ismap|loop|multiple|open|readonly|required|scoped",P="[\\x20\\t\\r\\n\\f]",R="(?:\\\\.|[\\w-]|[^\\x00-\\xa0])+",W=R.replace("w","w#"),$="\\["+P+"*("+R+")"+P+"*(?:([*^$|!~]?=)"+P+"*(?:(['\"])((?:\\\\.|[^\\\\])*?)\\3|("+W+")|)|)"+P+"*\\]",I=":("+R+")(?:\\(((['\"])((?:\\\\.|[^\\\\])*?)\\3|((?:\\\\.|[^\\\\()[\\]]|"+$.replace(3,8)+")*)|.*)\\)|)",z=RegExp("^"+P+"+|((?:^|[^\\\\])(?:\\\\.)*)"+P+"+$","g"),X=RegExp("^"+P+"*,"+P+"*"),U=RegExp("^"+P+"*([>+~]|"+P+")"+P+"*"),V=RegExp(P+"*[+~]"),Y=RegExp("="+P+"*([^\\]'\"]*)"+P+"*\\]","g"),J=RegExp(I),G=RegExp("^"+W+"$"),Q={ID:RegExp("^#("+R+")"),CLASS:RegExp("^\\.("+R+")"),TAG:RegExp("^("+R.replace("w","w*")+")"),ATTR:RegExp("^"+$),PSEUDO:RegExp("^"+I),CHILD:RegExp("^:(only|first|last|nth|nth-last)-(child|of-type)(?:\\("+P+"*(even|odd|(([+-]|)(\\d*)n|)"+P+"*(?:([+-]|)"+P+"*(\\d+)|))"+P+"*\\)|)","i"),bool:RegExp("^(?:"+B+")$","i"),needsContext:RegExp("^"+P+"*[>+~]|:(even|odd|eq|gt|lt|nth|first|last)(?:\\("+P+"*((?:-\\d)?\\d*)"+P+"*\\)|)(?=[^-]|$)","i")},K=/^[^{]+\{\s*\[native \w/,Z=/^(?:#([\w-]+)|(\w+)|\.([\w-]+))$/,et=/^(?:input|select|textarea|button)$/i,tt=/^h\d$/i,nt=/'|\\/g,rt=RegExp("\\\\([\\da-f]{1,6}"+P+"?|("+P+")|.)","ig"),it=function(e,t,n){var r="0x"+t-65536;return r!==r||n?t:0>r?String.fromCharCode(r+65536):String.fromCharCode(55296|r>>10,56320|1023&r)};try{M.apply(H=O.call(w.childNodes),w.childNodes),H[w.childNodes.length].nodeType}catch(ot){M={apply:H.length?function(e,t){_.apply(e,O.call(t))}:function(e,t){var n=e.length,r=0;while(e[n++]=t[r++]);e.length=n-1}}}function at(e,t,n,i){var o,a,s,l,u,c,d,m,y,x;if((t?t.ownerDocument||t:w)!==f&&p(t),t=t||f,n=n||[],!e||"string"!=typeof e)return n;if(1!==(l=t.nodeType)&&9!==l)return[];if(h&&!i){if(o=Z.exec(e))if(s=o[1]){if(9===l){if(a=t.getElementById(s),!a||!a.parentNode)return n;if(a.id===s)return n.push(a),n}else if(t.ownerDocument&&(a=t.ownerDocument.getElementById(s))&&v(t,a)&&a.id===s)return n.push(a),n}else{if(o[2])return M.apply(n,t.getElementsByTagName(e)),n;if((s=o[3])&&r.getElementsByClassName&&t.getElementsByClassName)return M.apply(n,t.getElementsByClassName(s)),n}if(r.qsa&&(!g||!g.test(e))){if(m=d=b,y=t,x=9===l&&e,1===l&&"object"!==t.nodeName.toLowerCase()){c=bt(e),(d=t.getAttribute("id"))?m=d.replace(nt,"\\$&"):t.setAttribute("id",m),m="[id='"+m+"'] ",u=c.length;while(u--)c[u]=m+xt(c[u]);y=V.test(e)&&t.parentNode||t,x=c.join(",")}if(x)try{return M.apply(n,y.querySelectorAll(x)),n}catch(T){}finally{d||t.removeAttribute("id")}}}return At(e.replace(z,"$1"),t,n,i)}function st(e){return K.test(e+"")}function lt(){var e=[];function t(n,r){return e.push(n+=" ")>o.cacheLength&&delete t[e.shift()],t[n]=r}return t}function ut(e){return e[b]=!0,e}function ct(e){var t=f.createElement("div");try{return!!e(t)}catch(n){return!1}finally{t.parentNode&&t.parentNode.removeChild(t),t=null}}function pt(e,t,n){e=e.split("|");var r,i=e.length,a=n?null:t;while(i--)(r=o.attrHandle[e[i]])&&r!==t||(o.attrHandle[e[i]]=a)}function ft(e,t){var n=e.getAttributeNode(t);return n&&n.specified?n.value:e[t]===!0?t.toLowerCase():null}function dt(e,t){return e.getAttribute(t,"type"===t.toLowerCase()?1:2)}function ht(e){return"input"===e.nodeName.toLowerCase()?e.defaultValue:t}function gt(e,t){var n=t&&e,r=n&&1===e.nodeType&&1===t.nodeType&&(~t.sourceIndex||D)-(~e.sourceIndex||D);if(r)return r;if(n)while(n=n.nextSibling)if(n===t)return-1;return e?1:-1}function mt(e){return function(t){var n=t.nodeName.toLowerCase();return"input"===n&&t.type===e}}function yt(e){return function(t){var n=t.nodeName.toLowerCase();return("input"===n||"button"===n)&&t.type===e}}function vt(e){return ut(function(t){return t=+t,ut(function(n,r){var i,o=e([],n.length,t),a=o.length;while(a--)n[i=o[a]]&&(n[i]=!(r[i]=n[i]))})})}s=at.isXML=function(e){var t=e&&(e.ownerDocument||e).documentElement;return t?"HTML"!==t.nodeName:!1},r=at.support={},p=at.setDocument=function(e){var n=e?e.ownerDocument||e:w,i=n.parentWindow;return n!==f&&9===n.nodeType&&n.documentElement?(f=n,d=n.documentElement,h=!s(n),i&&i.frameElement&&i.attachEvent("onbeforeunload",function(){p()}),r.attributes=ct(function(e){return e.innerHTML="<a href='#'></a>",pt("type|href|height|width",dt,"#"===e.firstChild.getAttribute("href")),pt(B,ft,null==e.getAttribute("disabled")),e.className="i",!e.getAttribute("className")}),r.input=ct(function(e){return e.innerHTML="<input>",e.firstChild.setAttribute("value",""),""===e.firstChild.getAttribute("value")}),pt("value",ht,r.attributes&&r.input),r.getElementsByTagName=ct(function(e){return e.appendChild(n.createComment("")),!e.getElementsByTagName("*").length}),r.getElementsByClassName=ct(function(e){return e.innerHTML="<div class='a'></div><div class='a i'></div>",e.firstChild.className="i",2===e.getElementsByClassName("i").length}),r.getById=ct(function(e){return d.appendChild(e).id=b,!n.getElementsByName||!n.getElementsByName(b).length}),r.getById?(o.find.ID=function(e,t){if(typeof t.getElementById!==j&&h){var n=t.getElementById(e);return n&&n.parentNode?[n]:[]}},o.filter.ID=function(e){var t=e.replace(rt,it);return function(e){return e.getAttribute("id")===t}}):(delete o.find.ID,o.filter.ID=function(e){var t=e.replace(rt,it);return function(e){var n=typeof e.getAttributeNode!==j&&e.getAttributeNode("id");return n&&n.value===t}}),o.find.TAG=r.getElementsByTagName?function(e,n){return typeof n.getElementsByTagName!==j?n.getElementsByTagName(e):t}:function(e,t){var n,r=[],i=0,o=t.getElementsByTagName(e);if("*"===e){while(n=o[i++])1===n.nodeType&&r.push(n);return r}return o},o.find.CLASS=r.getElementsByClassName&&function(e,n){return typeof n.getElementsByClassName!==j&&h?n.getElementsByClassName(e):t},m=[],g=[],(r.qsa=st(n.querySelectorAll))&&(ct(function(e){e.innerHTML="<select><option selected=''></option></select>",e.querySelectorAll("[selected]").length||g.push("\\["+P+"*(?:value|"+B+")"),e.querySelectorAll(":checked").length||g.push(":checked")}),ct(function(e){var t=n.createElement("input");t.setAttribute("type","hidden"),e.appendChild(t).setAttribute("t",""),e.querySelectorAll("[t^='']").length&&g.push("[*^$]="+P+"*(?:''|\"\")"),e.querySelectorAll(":enabled").length||g.push(":enabled",":disabled"),e.querySelectorAll("*,:x"),g.push(",.*:")})),(r.matchesSelector=st(y=d.webkitMatchesSelector||d.mozMatchesSelector||d.oMatchesSelector||d.msMatchesSelector))&&ct(function(e){r.disconnectedMatch=y.call(e,"div"),y.call(e,"[s!='']:x"),m.push("!=",I)}),g=g.length&&RegExp(g.join("|")),m=m.length&&RegExp(m.join("|")),v=st(d.contains)||d.compareDocumentPosition?function(e,t){var n=9===e.nodeType?e.documentElement:e,r=t&&t.parentNode;return e===r||!(!r||1!==r.nodeType||!(n.contains?n.contains(r):e.compareDocumentPosition&&16&e.compareDocumentPosition(r)))}:function(e,t){if(t)while(t=t.parentNode)if(t===e)return!0;return!1},r.sortDetached=ct(function(e){return 1&e.compareDocumentPosition(n.createElement("div"))}),A=d.compareDocumentPosition?function(e,t){if(e===t)return S=!0,0;var i=t.compareDocumentPosition&&e.compareDocumentPosition&&e.compareDocumentPosition(t);return i?1&i||!r.sortDetached&&t.compareDocumentPosition(e)===i?e===n||v(w,e)?-1:t===n||v(w,t)?1:c?F.call(c,e)-F.call(c,t):0:4&i?-1:1:e.compareDocumentPosition?-1:1}:function(e,t){var r,i=0,o=e.parentNode,a=t.parentNode,s=[e],l=[t];if(e===t)return S=!0,0;if(!o||!a)return e===n?-1:t===n?1:o?-1:a?1:c?F.call(c,e)-F.call(c,t):0;if(o===a)return gt(e,t);r=e;while(r=r.parentNode)s.unshift(r);r=t;while(r=r.parentNode)l.unshift(r);while(s[i]===l[i])i++;return i?gt(s[i],l[i]):s[i]===w?-1:l[i]===w?1:0},n):f},at.matches=function(e,t){return at(e,null,null,t)},at.matchesSelector=function(e,t){if((e.ownerDocument||e)!==f&&p(e),t=t.replace(Y,"='$1']"),!(!r.matchesSelector||!h||m&&m.test(t)||g&&g.test(t)))try{var n=y.call(e,t);if(n||r.disconnectedMatch||e.document&&11!==e.document.nodeType)return n}catch(i){}return at(t,f,null,[e]).length>0},at.contains=function(e,t){return(e.ownerDocument||e)!==f&&p(e),v(e,t)},at.attr=function(e,n){(e.ownerDocument||e)!==f&&p(e);var i=o.attrHandle[n.toLowerCase()],a=i&&L.call(o.attrHandle,n.toLowerCase())?i(e,n,!h):t;return a===t?r.attributes||!h?e.getAttribute(n):(a=e.getAttributeNode(n))&&a.specified?a.value:null:a},at.error=function(e){throw Error("Syntax error, unrecognized expression: "+e)},at.uniqueSort=function(e){var t,n=[],i=0,o=0;if(S=!r.detectDuplicates,c=!r.sortStable&&e.slice(0),e.sort(A),S){while(t=e[o++])t===e[o]&&(i=n.push(o));while(i--)e.splice(n[i],1)}return e},a=at.getText=function(e){var t,n="",r=0,i=e.nodeType;if(i){if(1===i||9===i||11===i){if("string"==typeof e.textContent)return e.textContent;for(e=e.firstChild;e;e=e.nextSibling)n+=a(e)}else if(3===i||4===i)return e.nodeValue}else for(;t=e[r];r++)n+=a(t);return n},o=at.selectors={cacheLength:50,createPseudo:ut,match:Q,attrHandle:{},find:{},relative:{">":{dir:"parentNode",first:!0}," ":{dir:"parentNode"},"+":{dir:"previousSibling",first:!0},"~":{dir:"previousSibling"}},preFilter:{ATTR:function(e){return e[1]=e[1].replace(rt,it),e[3]=(e[4]||e[5]||"").replace(rt,it),"~="===e[2]&&(e[3]=" "+e[3]+" "),e.slice(0,4)},CHILD:function(e){return e[1]=e[1].toLowerCase(),"nth"===e[1].slice(0,3)?(e[3]||at.error(e[0]),e[4]=+(e[4]?e[5]+(e[6]||1):2*("even"===e[3]||"odd"===e[3])),e[5]=+(e[7]+e[8]||"odd"===e[3])):e[3]&&at.error(e[0]),e},PSEUDO:function(e){var n,r=!e[5]&&e[2];return Q.CHILD.test(e[0])?null:(e[3]&&e[4]!==t?e[2]=e[4]:r&&J.test(r)&&(n=bt(r,!0))&&(n=r.indexOf(")",r.length-n)-r.length)&&(e[0]=e[0].slice(0,n),e[2]=r.slice(0,n)),e.slice(0,3))}},filter:{TAG:function(e){var t=e.replace(rt,it).toLowerCase();return"*"===e?function(){return!0}:function(e){return e.nodeName&&e.nodeName.toLowerCase()===t}},CLASS:function(e){var t=N[e+" "];return t||(t=RegExp("(^|"+P+")"+e+"("+P+"|$)"))&&N(e,function(e){return t.test("string"==typeof e.className&&e.className||typeof e.getAttribute!==j&&e.getAttribute("class")||"")})},ATTR:function(e,t,n){return function(r){var i=at.attr(r,e);return null==i?"!="===t:t?(i+="","="===t?i===n:"!="===t?i!==n:"^="===t?n&&0===i.indexOf(n):"*="===t?n&&i.indexOf(n)>-1:"$="===t?n&&i.slice(-n.length)===n:"~="===t?(" "+i+" ").indexOf(n)>-1:"|="===t?i===n||i.slice(0,n.length+1)===n+"-":!1):!0}},CHILD:function(e,t,n,r,i){var o="nth"!==e.slice(0,3),a="last"!==e.slice(-4),s="of-type"===t;return 1===r&&0===i?function(e){return!!e.parentNode}:function(t,n,l){var u,c,p,f,d,h,g=o!==a?"nextSibling":"previousSibling",m=t.parentNode,y=s&&t.nodeName.toLowerCase(),v=!l&&!s;if(m){if(o){while(g){p=t;while(p=p[g])if(s?p.nodeName.toLowerCase()===y:1===p.nodeType)return!1;h=g="only"===e&&!h&&"nextSibling"}return!0}if(h=[a?m.firstChild:m.lastChild],a&&v){c=m[b]||(m[b]={}),u=c[e]||[],d=u[0]===T&&u[1],f=u[0]===T&&u[2],p=d&&m.childNodes[d];while(p=++d&&p&&p[g]||(f=d=0)||h.pop())if(1===p.nodeType&&++f&&p===t){c[e]=[T,d,f];break}}else if(v&&(u=(t[b]||(t[b]={}))[e])&&u[0]===T)f=u[1];else while(p=++d&&p&&p[g]||(f=d=0)||h.pop())if((s?p.nodeName.toLowerCase()===y:1===p.nodeType)&&++f&&(v&&((p[b]||(p[b]={}))[e]=[T,f]),p===t))break;return f-=i,f===r||0===f%r&&f/r>=0}}},PSEUDO:function(e,t){var n,r=o.pseudos[e]||o.setFilters[e.toLowerCase()]||at.error("unsupported pseudo: "+e);return r[b]?r(t):r.length>1?(n=[e,e,"",t],o.setFilters.hasOwnProperty(e.toLowerCase())?ut(function(e,n){var i,o=r(e,t),a=o.length;while(a--)i=F.call(e,o[a]),e[i]=!(n[i]=o[a])}):function(e){return r(e,0,n)}):r}},pseudos:{not:ut(function(e){var t=[],n=[],r=l(e.replace(z,"$1"));return r[b]?ut(function(e,t,n,i){var o,a=r(e,null,i,[]),s=e.length;while(s--)(o=a[s])&&(e[s]=!(t[s]=o))}):function(e,i,o){return t[0]=e,r(t,null,o,n),!n.pop()}}),has:ut(function(e){return function(t){return at(e,t).length>0}}),contains:ut(function(e){return function(t){return(t.textContent||t.innerText||a(t)).indexOf(e)>-1}}),lang:ut(function(e){return G.test(e||"")||at.error("unsupported lang: "+e),e=e.replace(rt,it).toLowerCase(),function(t){var n;do if(n=h?t.lang:t.getAttribute("xml:lang")||t.getAttribute("lang"))return n=n.toLowerCase(),n===e||0===n.indexOf(e+"-");while((t=t.parentNode)&&1===t.nodeType);return!1}}),target:function(t){var n=e.location&&e.location.hash;return n&&n.slice(1)===t.id},root:function(e){return e===d},focus:function(e){return e===f.activeElement&&(!f.hasFocus||f.hasFocus())&&!!(e.type||e.href||~e.tabIndex)},enabled:function(e){return e.disabled===!1},disabled:function(e){return e.disabled===!0},checked:function(e){var t=e.nodeName.toLowerCase();return"input"===t&&!!e.checked||"option"===t&&!!e.selected},selected:function(e){return e.parentNode&&e.parentNode.selectedIndex,e.selected===!0},empty:function(e){for(e=e.firstChild;e;e=e.nextSibling)if(e.nodeName>"@"||3===e.nodeType||4===e.nodeType)return!1;return!0},parent:function(e){return!o.pseudos.empty(e)},header:function(e){return tt.test(e.nodeName)},input:function(e){return et.test(e.nodeName)},button:function(e){var t=e.nodeName.toLowerCase();return"input"===t&&"button"===e.type||"button"===t},text:function(e){var t;return"input"===e.nodeName.toLowerCase()&&"text"===e.type&&(null==(t=e.getAttribute("type"))||t.toLowerCase()===e.type)},first:vt(function(){return[0]}),last:vt(function(e,t){return[t-1]}),eq:vt(function(e,t,n){return[0>n?n+t:n]}),even:vt(function(e,t){var n=0;for(;t>n;n+=2)e.push(n);return e}),odd:vt(function(e,t){var n=1;for(;t>n;n+=2)e.push(n);return e}),lt:vt(function(e,t,n){var r=0>n?n+t:n;for(;--r>=0;)e.push(r);return e}),gt:vt(function(e,t,n){var r=0>n?n+t:n;for(;t>++r;)e.push(r);return e})}};for(n in{radio:!0,checkbox:!0,file:!0,password:!0,image:!0})o.pseudos[n]=mt(n);for(n in{submit:!0,reset:!0})o.pseudos[n]=yt(n);function bt(e,t){var n,r,i,a,s,l,u,c=k[e+" "];if(c)return t?0:c.slice(0);s=e,l=[],u=o.preFilter;while(s){(!n||(r=X.exec(s)))&&(r&&(s=s.slice(r[0].length)||s),l.push(i=[])),n=!1,(r=U.exec(s))&&(n=r.shift(),i.push({value:n,type:r[0].replace(z," ")}),s=s.slice(n.length));for(a in o.filter)!(r=Q[a].exec(s))||u[a]&&!(r=u[a](r))||(n=r.shift(),i.push({value:n,type:a,matches:r}),s=s.slice(n.length));if(!n)break}return t?s.length:s?at.error(e):k(e,l).slice(0)}function xt(e){var t=0,n=e.length,r="";for(;n>t;t++)r+=e[t].value;return r}function wt(e,t,n){var r=t.dir,o=n&&"parentNode"===r,a=C++;return t.first?function(t,n,i){while(t=t[r])if(1===t.nodeType||o)return e(t,n,i)}:function(t,n,s){var l,u,c,p=T+" "+a;if(s){while(t=t[r])if((1===t.nodeType||o)&&e(t,n,s))return!0}else while(t=t[r])if(1===t.nodeType||o)if(c=t[b]||(t[b]={}),(u=c[r])&&u[0]===p){if((l=u[1])===!0||l===i)return l===!0}else if(u=c[r]=[p],u[1]=e(t,n,s)||i,u[1]===!0)return!0}}function Tt(e){return e.length>1?function(t,n,r){var i=e.length;while(i--)if(!e[i](t,n,r))return!1;return!0}:e[0]}function Ct(e,t,n,r,i){var o,a=[],s=0,l=e.length,u=null!=t;for(;l>s;s++)(o=e[s])&&(!n||n(o,r,i))&&(a.push(o),u&&t.push(s));return a}function Nt(e,t,n,r,i,o){return r&&!r[b]&&(r=Nt(r)),i&&!i[b]&&(i=Nt(i,o)),ut(function(o,a,s,l){var u,c,p,f=[],d=[],h=a.length,g=o||St(t||"*",s.nodeType?[s]:s,[]),m=!e||!o&&t?g:Ct(g,f,e,s,l),y=n?i||(o?e:h||r)?[]:a:m;if(n&&n(m,y,s,l),r){u=Ct(y,d),r(u,[],s,l),c=u.length;while(c--)(p=u[c])&&(y[d[c]]=!(m[d[c]]=p))}if(o){if(i||e){if(i){u=[],c=y.length;while(c--)(p=y[c])&&u.push(m[c]=p);i(null,y=[],u,l)}c=y.length;while(c--)(p=y[c])&&(u=i?F.call(o,p):f[c])>-1&&(o[u]=!(a[u]=p))}}else y=Ct(y===a?y.splice(h,y.length):y),i?i(null,a,y,l):M.apply(a,y)})}function kt(e){var t,n,r,i=e.length,a=o.relative[e[0].type],s=a||o.relative[" "],l=a?1:0,c=wt(function(e){return e===t},s,!0),p=wt(function(e){return F.call(t,e)>-1},s,!0),f=[function(e,n,r){return!a&&(r||n!==u)||((t=n).nodeType?c(e,n,r):p(e,n,r))}];for(;i>l;l++)if(n=o.relative[e[l].type])f=[wt(Tt(f),n)];else{if(n=o.filter[e[l].type].apply(null,e[l].matches),n[b]){for(r=++l;i>r;r++)if(o.relative[e[r].type])break;return Nt(l>1&&Tt(f),l>1&&xt(e.slice(0,l-1).concat({value:" "===e[l-2].type?"*":""})).replace(z,"$1"),n,r>l&&kt(e.slice(l,r)),i>r&&kt(e=e.slice(r)),i>r&&xt(e))}f.push(n)}return Tt(f)}function Et(e,t){var n=0,r=t.length>0,a=e.length>0,s=function(s,l,c,p,d){var h,g,m,y=[],v=0,b="0",x=s&&[],w=null!=d,C=u,N=s||a&&o.find.TAG("*",d&&l.parentNode||l),k=T+=null==C?1:Math.random()||.1;for(w&&(u=l!==f&&l,i=n);null!=(h=N[b]);b++){if(a&&h){g=0;while(m=e[g++])if(m(h,l,c)){p.push(h);break}w&&(T=k,i=++n)}r&&((h=!m&&h)&&v--,s&&x.push(h))}if(v+=b,r&&b!==v){g=0;while(m=t[g++])m(x,y,l,c);if(s){if(v>0)while(b--)x[b]||y[b]||(y[b]=q.call(p));y=Ct(y)}M.apply(p,y),w&&!s&&y.length>0&&v+t.length>1&&at.uniqueSort(p)}return w&&(T=k,u=C),x};return r?ut(s):s}l=at.compile=function(e,t){var n,r=[],i=[],o=E[e+" "];if(!o){t||(t=bt(e)),n=t.length;while(n--)o=kt(t[n]),o[b]?r.push(o):i.push(o);o=E(e,Et(i,r))}return o};function St(e,t,n){var r=0,i=t.length;for(;i>r;r++)at(e,t[r],n);return n}function At(e,t,n,i){var a,s,u,c,p,f=bt(e);if(!i&&1===f.length){if(s=f[0]=f[0].slice(0),s.length>2&&"ID"===(u=s[0]).type&&r.getById&&9===t.nodeType&&h&&o.relative[s[1].type]){if(t=(o.find.ID(u.matches[0].replace(rt,it),t)||[])[0],!t)return n;e=e.slice(s.shift().value.length)}a=Q.needsContext.test(e)?0:s.length;while(a--){if(u=s[a],o.relative[c=u.type])break;if((p=o.find[c])&&(i=p(u.matches[0].replace(rt,it),V.test(s[0].type)&&t.parentNode||t))){if(s.splice(a,1),e=i.length&&xt(s),!e)return M.apply(n,i),n;break}}}return l(e,f)(i,t,!h,n,V.test(e)),n}o.pseudos.nth=o.pseudos.eq;function jt(){}jt.prototype=o.filters=o.pseudos,o.setFilters=new jt,r.sortStable=b.split("").sort(A).join("")===b,p(),[0,0].sort(A),r.detectDuplicates=S,x.find=at,x.expr=at.selectors,x.expr[":"]=x.expr.pseudos,x.unique=at.uniqueSort,x.text=at.getText,x.isXMLDoc=at.isXML,x.contains=at.contains}(e);var O={};function F(e){var t=O[e]={};return x.each(e.match(T)||[],function(e,n){t[n]=!0}),t}x.Callbacks=function(e){e="string"==typeof e?O[e]||F(e):x.extend({},e);var n,r,i,o,a,s,l=[],u=!e.once&&[],c=function(t){for(r=e.memory&&t,i=!0,a=s||0,s=0,o=l.length,n=!0;l&&o>a;a++)if(l[a].apply(t[0],t[1])===!1&&e.stopOnFalse){r=!1;break}n=!1,l&&(u?u.length&&c(u.shift()):r?l=[]:p.disable())},p={add:function(){if(l){var t=l.length;(function i(t){x.each(t,function(t,n){var r=x.type(n);"function"===r?e.unique&&p.has(n)||l.push(n):n&&n.length&&"string"!==r&&i(n)})})(arguments),n?o=l.length:r&&(s=t,c(r))}return this},remove:function(){return l&&x.each(arguments,function(e,t){var r;while((r=x.inArray(t,l,r))>-1)l.splice(r,1),n&&(o>=r&&o--,a>=r&&a--)}),this},has:function(e){return e?x.inArray(e,l)>-1:!(!l||!l.length)},empty:function(){return l=[],o=0,this},disable:function(){return l=u=r=t,this},disabled:function(){return!l},lock:function(){return u=t,r||p.disable(),this},locked:function(){return!u},fireWith:function(e,t){return t=t||[],t=[e,t.slice?t.slice():t],!l||i&&!u||(n?u.push(t):c(t)),this},fire:function(){return p.fireWith(this,arguments),this},fired:function(){return!!i}};return p},x.extend({Deferred:function(e){var t=[["resolve","done",x.Callbacks("once memory"),"resolved"],["reject","fail",x.Callbacks("once memory"),"rejected"],["notify","progress",x.Callbacks("memory")]],n="pending",r={state:function(){return n},always:function(){return i.done(arguments).fail(arguments),this},then:function(){var e=arguments;return x.Deferred(function(n){x.each(t,function(t,o){var a=o[0],s=x.isFunction(e[t])&&e[t];i[o[1]](function(){var e=s&&s.apply(this,arguments);e&&x.isFunction(e.promise)?e.promise().done(n.resolve).fail(n.reject).progress(n.notify):n[a+"With"](this===r?n.promise():this,s?[e]:arguments)})}),e=null}).promise()},promise:function(e){return null!=e?x.extend(e,r):r}},i={};return r.pipe=r.then,x.each(t,function(e,o){var a=o[2],s=o[3];r[o[1]]=a.add,s&&a.add(function(){n=s},t[1^e][2].disable,t[2][2].lock),i[o[0]]=function(){return i[o[0]+"With"](this===i?r:this,arguments),this},i[o[0]+"With"]=a.fireWith}),r.promise(i),e&&e.call(i,i),i},when:function(e){var t=0,n=g.call(arguments),r=n.length,i=1!==r||e&&x.isFunction(e.promise)?r:0,o=1===i?e:x.Deferred(),a=function(e,t,n){return function(r){t[e]=this,n[e]=arguments.length>1?g.call(arguments):r,n===s?o.notifyWith(t,n):--i||o.resolveWith(t,n)}},s,l,u;if(r>1)for(s=Array(r),l=Array(r),u=Array(r);r>t;t++)n[t]&&x.isFunction(n[t].promise)?n[t].promise().done(a(t,u,n)).fail(o.reject).progress(a(t,l,s)):--i;return i||o.resolveWith(u,n),o.promise()}}),x.support=function(t){var n,r,o,s,l,u,c,p,f,d=a.createElement("div");if(d.setAttribute("className","t"),d.innerHTML="  <link/><table></table><a href='/a'>a</a><input type='checkbox'/>",n=d.getElementsByTagName("*")||[],r=d.getElementsByTagName("a")[0],!r||!r.style||!n.length)return t;s=a.createElement("select"),u=s.appendChild(a.createElement("option")),o=d.getElementsByTagName("input")[0],r.style.cssText="top:1px;float:left;opacity:.5",t.getSetAttribute="t"!==d.className,t.leadingWhitespace=3===d.firstChild.nodeType,t.tbody=!d.getElementsByTagName("tbody").length,t.htmlSerialize=!!d.getElementsByTagName("link").length,t.style=/top/.test(r.getAttribute("style")),t.hrefNormalized="/a"===r.getAttribute("href"),t.opacity=/^0.5/.test(r.style.opacity),t.cssFloat=!!r.style.cssFloat,t.checkOn=!!o.value,t.optSelected=u.selected,t.enctype=!!a.createElement("form").enctype,t.html5Clone="<:nav></:nav>"!==a.createElement("nav").cloneNode(!0).outerHTML,t.inlineBlockNeedsLayout=!1,t.shrinkWrapBlocks=!1,t.pixelPosition=!1,t.deleteExpando=!0,t.noCloneEvent=!0,t.reliableMarginRight=!0,t.boxSizingReliable=!0,o.checked=!0,t.noCloneChecked=o.cloneNode(!0).checked,s.disabled=!0,t.optDisabled=!u.disabled;try{delete d.test}catch(h){t.deleteExpando=!1}o=a.createElement("input"),o.setAttribute("value",""),t.input=""===o.getAttribute("value"),o.value="t",o.setAttribute("type","radio"),t.radioValue="t"===o.value,o.setAttribute("checked","t"),o.setAttribute("name","t"),l=a.createDocumentFragment(),l.appendChild(o),t.appendChecked=o.checked,t.checkClone=l.cloneNode(!0).cloneNode(!0).lastChild.checked,d.attachEvent&&(d.attachEvent("onclick",function(){t.noCloneEvent=!1}),d.cloneNode(!0).click());for(f in{submit:!0,change:!0,focusin:!0})d.setAttribute(c="on"+f,"t"),t[f+"Bubbles"]=c in e||d.attributes[c].expando===!1;d.style.backgroundClip="content-box",d.cloneNode(!0).style.backgroundClip="",t.clearCloneStyle="content-box"===d.style.backgroundClip;for(f in x(t))break;return t.ownLast="0"!==f,x(function(){var n,r,o,s="padding:0;margin:0;border:0;display:block;box-sizing:content-box;-moz-box-sizing:content-box;-webkit-box-sizing:content-box;",l=a.getElementsByTagName("body")[0];l&&(n=a.createElement("div"),n.style.cssText="border:0;width:0;height:0;position:absolute;top:0;left:-9999px;margin-top:1px",l.appendChild(n).appendChild(d),d.innerHTML="<table><tr><td></td><td>t</td></tr></table>",o=d.getElementsByTagName("td"),o[0].style.cssText="padding:0;margin:0;border:0;display:none",p=0===o[0].offsetHeight,o[0].style.display="",o[1].style.display="none",t.reliableHiddenOffsets=p&&0===o[0].offsetHeight,d.innerHTML="",d.style.cssText="box-sizing:border-box;-moz-box-sizing:border-box;-webkit-box-sizing:border-box;padding:1px;border:1px;display:block;width:4px;margin-top:1%;position:absolute;top:1%;",x.swap(l,null!=l.style.zoom?{zoom:1}:{},function(){t.boxSizing=4===d.offsetWidth}),e.getComputedStyle&&(t.pixelPosition="1%"!==(e.getComputedStyle(d,null)||{}).top,t.boxSizingReliable="4px"===(e.getComputedStyle(d,null)||{width:"4px"}).width,r=d.appendChild(a.createElement("div")),r.style.cssText=d.style.cssText=s,r.style.marginRight=r.style.width="0",d.style.width="1px",t.reliableMarginRight=!parseFloat((e.getComputedStyle(r,null)||{}).marginRight)),typeof d.style.zoom!==i&&(d.innerHTML="",d.style.cssText=s+"width:1px;padding:1px;display:inline;zoom:1",t.inlineBlockNeedsLayout=3===d.offsetWidth,d.style.display="block",d.innerHTML="<div></div>",d.firstChild.style.width="5px",t.shrinkWrapBlocks=3!==d.offsetWidth,t.inlineBlockNeedsLayout&&(l.style.zoom=1)),l.removeChild(n),n=d=o=r=null)
}),n=s=l=u=r=o=null,t}({});var B=/(?:\{[\s\S]*\}|\[[\s\S]*\])$/,P=/([A-Z])/g;function R(e,n,r,i){if(x.acceptData(e)){var o,a,s=x.expando,l=e.nodeType,u=l?x.cache:e,c=l?e[s]:e[s]&&s;if(c&&u[c]&&(i||u[c].data)||r!==t||"string"!=typeof n)return c||(c=l?e[s]=p.pop()||x.guid++:s),u[c]||(u[c]=l?{}:{toJSON:x.noop}),("object"==typeof n||"function"==typeof n)&&(i?u[c]=x.extend(u[c],n):u[c].data=x.extend(u[c].data,n)),a=u[c],i||(a.data||(a.data={}),a=a.data),r!==t&&(a[x.camelCase(n)]=r),"string"==typeof n?(o=a[n],null==o&&(o=a[x.camelCase(n)])):o=a,o}}function W(e,t,n){if(x.acceptData(e)){var r,i,o=e.nodeType,a=o?x.cache:e,s=o?e[x.expando]:x.expando;if(a[s]){if(t&&(r=n?a[s]:a[s].data)){x.isArray(t)?t=t.concat(x.map(t,x.camelCase)):t in r?t=[t]:(t=x.camelCase(t),t=t in r?[t]:t.split(" ")),i=t.length;while(i--)delete r[t[i]];if(n?!I(r):!x.isEmptyObject(r))return}(n||(delete a[s].data,I(a[s])))&&(o?x.cleanData([e],!0):x.support.deleteExpando||a!=a.window?delete a[s]:a[s]=null)}}}x.extend({cache:{},noData:{applet:!0,embed:!0,object:"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000"},hasData:function(e){return e=e.nodeType?x.cache[e[x.expando]]:e[x.expando],!!e&&!I(e)},data:function(e,t,n){return R(e,t,n)},removeData:function(e,t){return W(e,t)},_data:function(e,t,n){return R(e,t,n,!0)},_removeData:function(e,t){return W(e,t,!0)},acceptData:function(e){if(e.nodeType&&1!==e.nodeType&&9!==e.nodeType)return!1;var t=e.nodeName&&x.noData[e.nodeName.toLowerCase()];return!t||t!==!0&&e.getAttribute("classid")===t}}),x.fn.extend({data:function(e,n){var r,i,o=null,a=0,s=this[0];if(e===t){if(this.length&&(o=x.data(s),1===s.nodeType&&!x._data(s,"parsedAttrs"))){for(r=s.attributes;r.length>a;a++)i=r[a].name,0===i.indexOf("data-")&&(i=x.camelCase(i.slice(5)),$(s,i,o[i]));x._data(s,"parsedAttrs",!0)}return o}return"object"==typeof e?this.each(function(){x.data(this,e)}):arguments.length>1?this.each(function(){x.data(this,e,n)}):s?$(s,e,x.data(s,e)):null},removeData:function(e){return this.each(function(){x.removeData(this,e)})}});function $(e,n,r){if(r===t&&1===e.nodeType){var i="data-"+n.replace(P,"-$1").toLowerCase();if(r=e.getAttribute(i),"string"==typeof r){try{r="true"===r?!0:"false"===r?!1:"null"===r?null:+r+""===r?+r:B.test(r)?x.parseJSON(r):r}catch(o){}x.data(e,n,r)}else r=t}return r}function I(e){var t;for(t in e)if(("data"!==t||!x.isEmptyObject(e[t]))&&"toJSON"!==t)return!1;return!0}x.extend({queue:function(e,n,r){var i;return e?(n=(n||"fx")+"queue",i=x._data(e,n),r&&(!i||x.isArray(r)?i=x._data(e,n,x.makeArray(r)):i.push(r)),i||[]):t},dequeue:function(e,t){t=t||"fx";var n=x.queue(e,t),r=n.length,i=n.shift(),o=x._queueHooks(e,t),a=function(){x.dequeue(e,t)};"inprogress"===i&&(i=n.shift(),r--),i&&("fx"===t&&n.unshift("inprogress"),delete o.stop,i.call(e,a,o)),!r&&o&&o.empty.fire()},_queueHooks:function(e,t){var n=t+"queueHooks";return x._data(e,n)||x._data(e,n,{empty:x.Callbacks("once memory").add(function(){x._removeData(e,t+"queue"),x._removeData(e,n)})})}}),x.fn.extend({queue:function(e,n){var r=2;return"string"!=typeof e&&(n=e,e="fx",r--),r>arguments.length?x.queue(this[0],e):n===t?this:this.each(function(){var t=x.queue(this,e,n);x._queueHooks(this,e),"fx"===e&&"inprogress"!==t[0]&&x.dequeue(this,e)})},dequeue:function(e){return this.each(function(){x.dequeue(this,e)})},delay:function(e,t){return e=x.fx?x.fx.speeds[e]||e:e,t=t||"fx",this.queue(t,function(t,n){var r=setTimeout(t,e);n.stop=function(){clearTimeout(r)}})},clearQueue:function(e){return this.queue(e||"fx",[])},promise:function(e,n){var r,i=1,o=x.Deferred(),a=this,s=this.length,l=function(){--i||o.resolveWith(a,[a])};"string"!=typeof e&&(n=e,e=t),e=e||"fx";while(s--)r=x._data(a[s],e+"queueHooks"),r&&r.empty&&(i++,r.empty.add(l));return l(),o.promise(n)}});var z,X,U=/[\t\r\n\f]/g,V=/\r/g,Y=/^(?:input|select|textarea|button|object)$/i,J=/^(?:a|area)$/i,G=/^(?:checked|selected)$/i,Q=x.support.getSetAttribute,K=x.support.input;x.fn.extend({attr:function(e,t){return x.access(this,x.attr,e,t,arguments.length>1)},removeAttr:function(e){return this.each(function(){x.removeAttr(this,e)})},prop:function(e,t){return x.access(this,x.prop,e,t,arguments.length>1)},removeProp:function(e){return e=x.propFix[e]||e,this.each(function(){try{this[e]=t,delete this[e]}catch(n){}})},addClass:function(e){var t,n,r,i,o,a=0,s=this.length,l="string"==typeof e&&e;if(x.isFunction(e))return this.each(function(t){x(this).addClass(e.call(this,t,this.className))});if(l)for(t=(e||"").match(T)||[];s>a;a++)if(n=this[a],r=1===n.nodeType&&(n.className?(" "+n.className+" ").replace(U," "):" ")){o=0;while(i=t[o++])0>r.indexOf(" "+i+" ")&&(r+=i+" ");n.className=x.trim(r)}return this},removeClass:function(e){var t,n,r,i,o,a=0,s=this.length,l=0===arguments.length||"string"==typeof e&&e;if(x.isFunction(e))return this.each(function(t){x(this).removeClass(e.call(this,t,this.className))});if(l)for(t=(e||"").match(T)||[];s>a;a++)if(n=this[a],r=1===n.nodeType&&(n.className?(" "+n.className+" ").replace(U," "):"")){o=0;while(i=t[o++])while(r.indexOf(" "+i+" ")>=0)r=r.replace(" "+i+" "," ");n.className=e?x.trim(r):""}return this},toggleClass:function(e,t){var n=typeof e,r="boolean"==typeof t;return x.isFunction(e)?this.each(function(n){x(this).toggleClass(e.call(this,n,this.className,t),t)}):this.each(function(){if("string"===n){var o,a=0,s=x(this),l=t,u=e.match(T)||[];while(o=u[a++])l=r?l:!s.hasClass(o),s[l?"addClass":"removeClass"](o)}else(n===i||"boolean"===n)&&(this.className&&x._data(this,"__className__",this.className),this.className=this.className||e===!1?"":x._data(this,"__className__")||"")})},hasClass:function(e){var t=" "+e+" ",n=0,r=this.length;for(;r>n;n++)if(1===this[n].nodeType&&(" "+this[n].className+" ").replace(U," ").indexOf(t)>=0)return!0;return!1},val:function(e){var n,r,i,o=this[0];{if(arguments.length)return i=x.isFunction(e),this.each(function(n){var o;1===this.nodeType&&(o=i?e.call(this,n,x(this).val()):e,null==o?o="":"number"==typeof o?o+="":x.isArray(o)&&(o=x.map(o,function(e){return null==e?"":e+""})),r=x.valHooks[this.type]||x.valHooks[this.nodeName.toLowerCase()],r&&"set"in r&&r.set(this,o,"value")!==t||(this.value=o))});if(o)return r=x.valHooks[o.type]||x.valHooks[o.nodeName.toLowerCase()],r&&"get"in r&&(n=r.get(o,"value"))!==t?n:(n=o.value,"string"==typeof n?n.replace(V,""):null==n?"":n)}}}),x.extend({valHooks:{option:{get:function(e){var t=x.find.attr(e,"value");return null!=t?t:e.text}},select:{get:function(e){var t,n,r=e.options,i=e.selectedIndex,o="select-one"===e.type||0>i,a=o?null:[],s=o?i+1:r.length,l=0>i?s:o?i:0;for(;s>l;l++)if(n=r[l],!(!n.selected&&l!==i||(x.support.optDisabled?n.disabled:null!==n.getAttribute("disabled"))||n.parentNode.disabled&&x.nodeName(n.parentNode,"optgroup"))){if(t=x(n).val(),o)return t;a.push(t)}return a},set:function(e,t){var n,r,i=e.options,o=x.makeArray(t),a=i.length;while(a--)r=i[a],(r.selected=x.inArray(x(r).val(),o)>=0)&&(n=!0);return n||(e.selectedIndex=-1),o}}},attr:function(e,n,r){var o,a,s=e.nodeType;if(e&&3!==s&&8!==s&&2!==s)return typeof e.getAttribute===i?x.prop(e,n,r):(1===s&&x.isXMLDoc(e)||(n=n.toLowerCase(),o=x.attrHooks[n]||(x.expr.match.bool.test(n)?X:z)),r===t?o&&"get"in o&&null!==(a=o.get(e,n))?a:(a=x.find.attr(e,n),null==a?t:a):null!==r?o&&"set"in o&&(a=o.set(e,r,n))!==t?a:(e.setAttribute(n,r+""),r):(x.removeAttr(e,n),t))},removeAttr:function(e,t){var n,r,i=0,o=t&&t.match(T);if(o&&1===e.nodeType)while(n=o[i++])r=x.propFix[n]||n,x.expr.match.bool.test(n)?K&&Q||!G.test(n)?e[r]=!1:e[x.camelCase("default-"+n)]=e[r]=!1:x.attr(e,n,""),e.removeAttribute(Q?n:r)},attrHooks:{type:{set:function(e,t){if(!x.support.radioValue&&"radio"===t&&x.nodeName(e,"input")){var n=e.value;return e.setAttribute("type",t),n&&(e.value=n),t}}}},propFix:{"for":"htmlFor","class":"className"},prop:function(e,n,r){var i,o,a,s=e.nodeType;if(e&&3!==s&&8!==s&&2!==s)return a=1!==s||!x.isXMLDoc(e),a&&(n=x.propFix[n]||n,o=x.propHooks[n]),r!==t?o&&"set"in o&&(i=o.set(e,r,n))!==t?i:e[n]=r:o&&"get"in o&&null!==(i=o.get(e,n))?i:e[n]},propHooks:{tabIndex:{get:function(e){var t=x.find.attr(e,"tabindex");return t?parseInt(t,10):Y.test(e.nodeName)||J.test(e.nodeName)&&e.href?0:-1}}}}),X={set:function(e,t,n){return t===!1?x.removeAttr(e,n):K&&Q||!G.test(n)?e.setAttribute(!Q&&x.propFix[n]||n,n):e[x.camelCase("default-"+n)]=e[n]=!0,n}},x.each(x.expr.match.bool.source.match(/\w+/g),function(e,n){var r=x.expr.attrHandle[n]||x.find.attr;x.expr.attrHandle[n]=K&&Q||!G.test(n)?function(e,n,i){var o=x.expr.attrHandle[n],a=i?t:(x.expr.attrHandle[n]=t)!=r(e,n,i)?n.toLowerCase():null;return x.expr.attrHandle[n]=o,a}:function(e,n,r){return r?t:e[x.camelCase("default-"+n)]?n.toLowerCase():null}}),K&&Q||(x.attrHooks.value={set:function(e,n,r){return x.nodeName(e,"input")?(e.defaultValue=n,t):z&&z.set(e,n,r)}}),Q||(z={set:function(e,n,r){var i=e.getAttributeNode(r);return i||e.setAttributeNode(i=e.ownerDocument.createAttribute(r)),i.value=n+="","value"===r||n===e.getAttribute(r)?n:t}},x.expr.attrHandle.id=x.expr.attrHandle.name=x.expr.attrHandle.coords=function(e,n,r){var i;return r?t:(i=e.getAttributeNode(n))&&""!==i.value?i.value:null},x.valHooks.button={get:function(e,n){var r=e.getAttributeNode(n);return r&&r.specified?r.value:t},set:z.set},x.attrHooks.contenteditable={set:function(e,t,n){z.set(e,""===t?!1:t,n)}},x.each(["width","height"],function(e,n){x.attrHooks[n]={set:function(e,r){return""===r?(e.setAttribute(n,"auto"),r):t}}})),x.support.hrefNormalized||x.each(["href","src"],function(e,t){x.propHooks[t]={get:function(e){return e.getAttribute(t,4)}}}),x.support.style||(x.attrHooks.style={get:function(e){return e.style.cssText||t},set:function(e,t){return e.style.cssText=t+""}}),x.support.optSelected||(x.propHooks.selected={get:function(e){var t=e.parentNode;return t&&(t.selectedIndex,t.parentNode&&t.parentNode.selectedIndex),null}}),x.each(["tabIndex","readOnly","maxLength","cellSpacing","cellPadding","rowSpan","colSpan","useMap","frameBorder","contentEditable"],function(){x.propFix[this.toLowerCase()]=this}),x.support.enctype||(x.propFix.enctype="encoding"),x.each(["radio","checkbox"],function(){x.valHooks[this]={set:function(e,n){return x.isArray(n)?e.checked=x.inArray(x(e).val(),n)>=0:t}},x.support.checkOn||(x.valHooks[this].get=function(e){return null===e.getAttribute("value")?"on":e.value})});var Z=/^(?:input|select|textarea)$/i,et=/^key/,tt=/^(?:mouse|contextmenu)|click/,nt=/^(?:focusinfocus|focusoutblur)$/,rt=/^([^.]*)(?:\.(.+)|)$/;function it(){return!0}function ot(){return!1}function at(){try{return a.activeElement}catch(e){}}x.event={global:{},add:function(e,n,r,o,a){var s,l,u,c,p,f,d,h,g,m,y,v=x._data(e);if(v){r.handler&&(c=r,r=c.handler,a=c.selector),r.guid||(r.guid=x.guid++),(l=v.events)||(l=v.events={}),(f=v.handle)||(f=v.handle=function(e){return typeof x===i||e&&x.event.triggered===e.type?t:x.event.dispatch.apply(f.elem,arguments)},f.elem=e),n=(n||"").match(T)||[""],u=n.length;while(u--)s=rt.exec(n[u])||[],g=y=s[1],m=(s[2]||"").split(".").sort(),g&&(p=x.event.special[g]||{},g=(a?p.delegateType:p.bindType)||g,p=x.event.special[g]||{},d=x.extend({type:g,origType:y,data:o,handler:r,guid:r.guid,selector:a,needsContext:a&&x.expr.match.needsContext.test(a),namespace:m.join(".")},c),(h=l[g])||(h=l[g]=[],h.delegateCount=0,p.setup&&p.setup.call(e,o,m,f)!==!1||(e.addEventListener?e.addEventListener(g,f,!1):e.attachEvent&&e.attachEvent("on"+g,f))),p.add&&(p.add.call(e,d),d.handler.guid||(d.handler.guid=r.guid)),a?h.splice(h.delegateCount++,0,d):h.push(d),x.event.global[g]=!0);e=null}},remove:function(e,t,n,r,i){var o,a,s,l,u,c,p,f,d,h,g,m=x.hasData(e)&&x._data(e);if(m&&(c=m.events)){t=(t||"").match(T)||[""],u=t.length;while(u--)if(s=rt.exec(t[u])||[],d=g=s[1],h=(s[2]||"").split(".").sort(),d){p=x.event.special[d]||{},d=(r?p.delegateType:p.bindType)||d,f=c[d]||[],s=s[2]&&RegExp("(^|\\.)"+h.join("\\.(?:.*\\.|)")+"(\\.|$)"),l=o=f.length;while(o--)a=f[o],!i&&g!==a.origType||n&&n.guid!==a.guid||s&&!s.test(a.namespace)||r&&r!==a.selector&&("**"!==r||!a.selector)||(f.splice(o,1),a.selector&&f.delegateCount--,p.remove&&p.remove.call(e,a));l&&!f.length&&(p.teardown&&p.teardown.call(e,h,m.handle)!==!1||x.removeEvent(e,d,m.handle),delete c[d])}else for(d in c)x.event.remove(e,d+t[u],n,r,!0);x.isEmptyObject(c)&&(delete m.handle,x._removeData(e,"events"))}},trigger:function(n,r,i,o){var s,l,u,c,p,f,d,h=[i||a],g=v.call(n,"type")?n.type:n,m=v.call(n,"namespace")?n.namespace.split("."):[];if(u=f=i=i||a,3!==i.nodeType&&8!==i.nodeType&&!nt.test(g+x.event.triggered)&&(g.indexOf(".")>=0&&(m=g.split("."),g=m.shift(),m.sort()),l=0>g.indexOf(":")&&"on"+g,n=n[x.expando]?n:new x.Event(g,"object"==typeof n&&n),n.isTrigger=o?2:3,n.namespace=m.join("."),n.namespace_re=n.namespace?RegExp("(^|\\.)"+m.join("\\.(?:.*\\.|)")+"(\\.|$)"):null,n.result=t,n.target||(n.target=i),r=null==r?[n]:x.makeArray(r,[n]),p=x.event.special[g]||{},o||!p.trigger||p.trigger.apply(i,r)!==!1)){if(!o&&!p.noBubble&&!x.isWindow(i)){for(c=p.delegateType||g,nt.test(c+g)||(u=u.parentNode);u;u=u.parentNode)h.push(u),f=u;f===(i.ownerDocument||a)&&h.push(f.defaultView||f.parentWindow||e)}d=0;while((u=h[d++])&&!n.isPropagationStopped())n.type=d>1?c:p.bindType||g,s=(x._data(u,"events")||{})[n.type]&&x._data(u,"handle"),s&&s.apply(u,r),s=l&&u[l],s&&x.acceptData(u)&&s.apply&&s.apply(u,r)===!1&&n.preventDefault();if(n.type=g,!o&&!n.isDefaultPrevented()&&(!p._default||p._default.apply(h.pop(),r)===!1)&&x.acceptData(i)&&l&&i[g]&&!x.isWindow(i)){f=i[l],f&&(i[l]=null),x.event.triggered=g;try{i[g]()}catch(y){}x.event.triggered=t,f&&(i[l]=f)}return n.result}},dispatch:function(e){e=x.event.fix(e);var n,r,i,o,a,s=[],l=g.call(arguments),u=(x._data(this,"events")||{})[e.type]||[],c=x.event.special[e.type]||{};if(l[0]=e,e.delegateTarget=this,!c.preDispatch||c.preDispatch.call(this,e)!==!1){s=x.event.handlers.call(this,e,u),n=0;while((o=s[n++])&&!e.isPropagationStopped()){e.currentTarget=o.elem,a=0;while((i=o.handlers[a++])&&!e.isImmediatePropagationStopped())(!e.namespace_re||e.namespace_re.test(i.namespace))&&(e.handleObj=i,e.data=i.data,r=((x.event.special[i.origType]||{}).handle||i.handler).apply(o.elem,l),r!==t&&(e.result=r)===!1&&(e.preventDefault(),e.stopPropagation()))}return c.postDispatch&&c.postDispatch.call(this,e),e.result}},handlers:function(e,n){var r,i,o,a,s=[],l=n.delegateCount,u=e.target;if(l&&u.nodeType&&(!e.button||"click"!==e.type))for(;u!=this;u=u.parentNode||this)if(1===u.nodeType&&(u.disabled!==!0||"click"!==e.type)){for(o=[],a=0;l>a;a++)i=n[a],r=i.selector+" ",o[r]===t&&(o[r]=i.needsContext?x(r,this).index(u)>=0:x.find(r,this,null,[u]).length),o[r]&&o.push(i);o.length&&s.push({elem:u,handlers:o})}return n.length>l&&s.push({elem:this,handlers:n.slice(l)}),s},fix:function(e){if(e[x.expando])return e;var t,n,r,i=e.type,o=e,s=this.fixHooks[i];s||(this.fixHooks[i]=s=tt.test(i)?this.mouseHooks:et.test(i)?this.keyHooks:{}),r=s.props?this.props.concat(s.props):this.props,e=new x.Event(o),t=r.length;while(t--)n=r[t],e[n]=o[n];return e.target||(e.target=o.srcElement||a),3===e.target.nodeType&&(e.target=e.target.parentNode),e.metaKey=!!e.metaKey,s.filter?s.filter(e,o):e},props:"altKey bubbles cancelable ctrlKey currentTarget eventPhase metaKey relatedTarget shiftKey target timeStamp view which".split(" "),fixHooks:{},keyHooks:{props:"char charCode key keyCode".split(" "),filter:function(e,t){return null==e.which&&(e.which=null!=t.charCode?t.charCode:t.keyCode),e}},mouseHooks:{props:"button buttons clientX clientY fromElement offsetX offsetY pageX pageY screenX screenY toElement".split(" "),filter:function(e,n){var r,i,o,s=n.button,l=n.fromElement;return null==e.pageX&&null!=n.clientX&&(i=e.target.ownerDocument||a,o=i.documentElement,r=i.body,e.pageX=n.clientX+(o&&o.scrollLeft||r&&r.scrollLeft||0)-(o&&o.clientLeft||r&&r.clientLeft||0),e.pageY=n.clientY+(o&&o.scrollTop||r&&r.scrollTop||0)-(o&&o.clientTop||r&&r.clientTop||0)),!e.relatedTarget&&l&&(e.relatedTarget=l===e.target?n.toElement:l),e.which||s===t||(e.which=1&s?1:2&s?3:4&s?2:0),e}},special:{load:{noBubble:!0},focus:{trigger:function(){if(this!==at()&&this.focus)try{return this.focus(),!1}catch(e){}},delegateType:"focusin"},blur:{trigger:function(){return this===at()&&this.blur?(this.blur(),!1):t},delegateType:"focusout"},click:{trigger:function(){return x.nodeName(this,"input")&&"checkbox"===this.type&&this.click?(this.click(),!1):t},_default:function(e){return x.nodeName(e.target,"a")}},beforeunload:{postDispatch:function(e){e.result!==t&&(e.originalEvent.returnValue=e.result)}}},simulate:function(e,t,n,r){var i=x.extend(new x.Event,n,{type:e,isSimulated:!0,originalEvent:{}});r?x.event.trigger(i,null,t):x.event.dispatch.call(t,i),i.isDefaultPrevented()&&n.preventDefault()}},x.removeEvent=a.removeEventListener?function(e,t,n){e.removeEventListener&&e.removeEventListener(t,n,!1)}:function(e,t,n){var r="on"+t;e.detachEvent&&(typeof e[r]===i&&(e[r]=null),e.detachEvent(r,n))},x.Event=function(e,n){return this instanceof x.Event?(e&&e.type?(this.originalEvent=e,this.type=e.type,this.isDefaultPrevented=e.defaultPrevented||e.returnValue===!1||e.getPreventDefault&&e.getPreventDefault()?it:ot):this.type=e,n&&x.extend(this,n),this.timeStamp=e&&e.timeStamp||x.now(),this[x.expando]=!0,t):new x.Event(e,n)},x.Event.prototype={isDefaultPrevented:ot,isPropagationStopped:ot,isImmediatePropagationStopped:ot,preventDefault:function(){var e=this.originalEvent;this.isDefaultPrevented=it,e&&(e.preventDefault?e.preventDefault():e.returnValue=!1)},stopPropagation:function(){var e=this.originalEvent;this.isPropagationStopped=it,e&&(e.stopPropagation&&e.stopPropagation(),e.cancelBubble=!0)},stopImmediatePropagation:function(){this.isImmediatePropagationStopped=it,this.stopPropagation()}},x.each({mouseenter:"mouseover",mouseleave:"mouseout"},function(e,t){x.event.special[e]={delegateType:t,bindType:t,handle:function(e){var n,r=this,i=e.relatedTarget,o=e.handleObj;return(!i||i!==r&&!x.contains(r,i))&&(e.type=o.origType,n=o.handler.apply(this,arguments),e.type=t),n}}}),x.support.submitBubbles||(x.event.special.submit={setup:function(){return x.nodeName(this,"form")?!1:(x.event.add(this,"click._submit keypress._submit",function(e){var n=e.target,r=x.nodeName(n,"input")||x.nodeName(n,"button")?n.form:t;r&&!x._data(r,"submitBubbles")&&(x.event.add(r,"submit._submit",function(e){e._submit_bubble=!0}),x._data(r,"submitBubbles",!0))}),t)},postDispatch:function(e){e._submit_bubble&&(delete e._submit_bubble,this.parentNode&&!e.isTrigger&&x.event.simulate("submit",this.parentNode,e,!0))},teardown:function(){return x.nodeName(this,"form")?!1:(x.event.remove(this,"._submit"),t)}}),x.support.changeBubbles||(x.event.special.change={setup:function(){return Z.test(this.nodeName)?(("checkbox"===this.type||"radio"===this.type)&&(x.event.add(this,"propertychange._change",function(e){"checked"===e.originalEvent.propertyName&&(this._just_changed=!0)}),x.event.add(this,"click._change",function(e){this._just_changed&&!e.isTrigger&&(this._just_changed=!1),x.event.simulate("change",this,e,!0)})),!1):(x.event.add(this,"beforeactivate._change",function(e){var t=e.target;Z.test(t.nodeName)&&!x._data(t,"changeBubbles")&&(x.event.add(t,"change._change",function(e){!this.parentNode||e.isSimulated||e.isTrigger||x.event.simulate("change",this.parentNode,e,!0)}),x._data(t,"changeBubbles",!0))}),t)},handle:function(e){var n=e.target;return this!==n||e.isSimulated||e.isTrigger||"radio"!==n.type&&"checkbox"!==n.type?e.handleObj.handler.apply(this,arguments):t},teardown:function(){return x.event.remove(this,"._change"),!Z.test(this.nodeName)}}),x.support.focusinBubbles||x.each({focus:"focusin",blur:"focusout"},function(e,t){var n=0,r=function(e){x.event.simulate(t,e.target,x.event.fix(e),!0)};x.event.special[t]={setup:function(){0===n++&&a.addEventListener(e,r,!0)},teardown:function(){0===--n&&a.removeEventListener(e,r,!0)}}}),x.fn.extend({on:function(e,n,r,i,o){var a,s;if("object"==typeof e){"string"!=typeof n&&(r=r||n,n=t);for(a in e)this.on(a,n,r,e[a],o);return this}if(null==r&&null==i?(i=n,r=n=t):null==i&&("string"==typeof n?(i=r,r=t):(i=r,r=n,n=t)),i===!1)i=ot;else if(!i)return this;return 1===o&&(s=i,i=function(e){return x().off(e),s.apply(this,arguments)},i.guid=s.guid||(s.guid=x.guid++)),this.each(function(){x.event.add(this,e,i,r,n)})},one:function(e,t,n,r){return this.on(e,t,n,r,1)},off:function(e,n,r){var i,o;if(e&&e.preventDefault&&e.handleObj)return i=e.handleObj,x(e.delegateTarget).off(i.namespace?i.origType+"."+i.namespace:i.origType,i.selector,i.handler),this;if("object"==typeof e){for(o in e)this.off(o,n,e[o]);return this}return(n===!1||"function"==typeof n)&&(r=n,n=t),r===!1&&(r=ot),this.each(function(){x.event.remove(this,e,r,n)})},trigger:function(e,t){return this.each(function(){x.event.trigger(e,t,this)})},triggerHandler:function(e,n){var r=this[0];return r?x.event.trigger(e,n,r,!0):t}});var st=/^.[^:#\[\.,]*$/,lt=/^(?:parents|prev(?:Until|All))/,ut=x.expr.match.needsContext,ct={children:!0,contents:!0,next:!0,prev:!0};x.fn.extend({find:function(e){var t,n=[],r=this,i=r.length;if("string"!=typeof e)return this.pushStack(x(e).filter(function(){for(t=0;i>t;t++)if(x.contains(r[t],this))return!0}));for(t=0;i>t;t++)x.find(e,r[t],n);return n=this.pushStack(i>1?x.unique(n):n),n.selector=this.selector?this.selector+" "+e:e,n},has:function(e){var t,n=x(e,this),r=n.length;return this.filter(function(){for(t=0;r>t;t++)if(x.contains(this,n[t]))return!0})},not:function(e){return this.pushStack(ft(this,e||[],!0))},filter:function(e){return this.pushStack(ft(this,e||[],!1))},is:function(e){return!!ft(this,"string"==typeof e&&ut.test(e)?x(e):e||[],!1).length},closest:function(e,t){var n,r=0,i=this.length,o=[],a=ut.test(e)||"string"!=typeof e?x(e,t||this.context):0;for(;i>r;r++)for(n=this[r];n&&n!==t;n=n.parentNode)if(11>n.nodeType&&(a?a.index(n)>-1:1===n.nodeType&&x.find.matchesSelector(n,e))){n=o.push(n);break}return this.pushStack(o.length>1?x.unique(o):o)},index:function(e){return e?"string"==typeof e?x.inArray(this[0],x(e)):x.inArray(e.jquery?e[0]:e,this):this[0]&&this[0].parentNode?this.first().prevAll().length:-1},add:function(e,t){var n="string"==typeof e?x(e,t):x.makeArray(e&&e.nodeType?[e]:e),r=x.merge(this.get(),n);return this.pushStack(x.unique(r))},addBack:function(e){return this.add(null==e?this.prevObject:this.prevObject.filter(e))}});function pt(e,t){do e=e[t];while(e&&1!==e.nodeType);return e}x.each({parent:function(e){var t=e.parentNode;return t&&11!==t.nodeType?t:null},parents:function(e){return x.dir(e,"parentNode")},parentsUntil:function(e,t,n){return x.dir(e,"parentNode",n)},next:function(e){return pt(e,"nextSibling")},prev:function(e){return pt(e,"previousSibling")},nextAll:function(e){return x.dir(e,"nextSibling")},prevAll:function(e){return x.dir(e,"previousSibling")},nextUntil:function(e,t,n){return x.dir(e,"nextSibling",n)},prevUntil:function(e,t,n){return x.dir(e,"previousSibling",n)},siblings:function(e){return x.sibling((e.parentNode||{}).firstChild,e)},children:function(e){return x.sibling(e.firstChild)},contents:function(e){return x.nodeName(e,"iframe")?e.contentDocument||e.contentWindow.document:x.merge([],e.childNodes)}},function(e,t){x.fn[e]=function(n,r){var i=x.map(this,t,n);return"Until"!==e.slice(-5)&&(r=n),r&&"string"==typeof r&&(i=x.filter(r,i)),this.length>1&&(ct[e]||(i=x.unique(i)),lt.test(e)&&(i=i.reverse())),this.pushStack(i)}}),x.extend({filter:function(e,t,n){var r=t[0];return n&&(e=":not("+e+")"),1===t.length&&1===r.nodeType?x.find.matchesSelector(r,e)?[r]:[]:x.find.matches(e,x.grep(t,function(e){return 1===e.nodeType}))},dir:function(e,n,r){var i=[],o=e[n];while(o&&9!==o.nodeType&&(r===t||1!==o.nodeType||!x(o).is(r)))1===o.nodeType&&i.push(o),o=o[n];return i},sibling:function(e,t){var n=[];for(;e;e=e.nextSibling)1===e.nodeType&&e!==t&&n.push(e);return n}});function ft(e,t,n){if(x.isFunction(t))return x.grep(e,function(e,r){return!!t.call(e,r,e)!==n});if(t.nodeType)return x.grep(e,function(e){return e===t!==n});if("string"==typeof t){if(st.test(t))return x.filter(t,e,n);t=x.filter(t,e)}return x.grep(e,function(e){return x.inArray(e,t)>=0!==n})}function dt(e){var t=ht.split("|"),n=e.createDocumentFragment();if(n.createElement)while(t.length)n.createElement(t.pop());return n}var ht="abbr|article|aside|audio|bdi|canvas|data|datalist|details|figcaption|figure|footer|header|hgroup|mark|meter|nav|output|progress|section|summary|time|video",gt=/ jQuery\d+="(?:null|\d+)"/g,mt=RegExp("<(?:"+ht+")[\\s/>]","i"),yt=/^\s+/,vt=/<(?!area|br|col|embed|hr|img|input|link|meta|param)(([\w:]+)[^>]*)\/>/gi,bt=/<([\w:]+)/,xt=/<tbody/i,wt=/<|&#?\w+;/,Tt=/<(?:script|style|link)/i,Ct=/^(?:checkbox|radio)$/i,Nt=/checked\s*(?:[^=]|=\s*.checked.)/i,kt=/^$|\/(?:java|ecma)script/i,Et=/^true\/(.*)/,St=/^\s*<!(?:\[CDATA\[|--)|(?:\]\]|--)>\s*$/g,At={option:[1,"<select multiple='multiple'>","</select>"],legend:[1,"<fieldset>","</fieldset>"],area:[1,"<map>","</map>"],param:[1,"<object>","</object>"],thead:[1,"<table>","</table>"],tr:[2,"<table><tbody>","</tbody></table>"],col:[2,"<table><tbody></tbody><colgroup>","</colgroup></table>"],td:[3,"<table><tbody><tr>","</tr></tbody></table>"],_default:x.support.htmlSerialize?[0,"",""]:[1,"X<div>","</div>"]},jt=dt(a),Dt=jt.appendChild(a.createElement("div"));At.optgroup=At.option,At.tbody=At.tfoot=At.colgroup=At.caption=At.thead,At.th=At.td,x.fn.extend({text:function(e){return x.access(this,function(e){return e===t?x.text(this):this.empty().append((this[0]&&this[0].ownerDocument||a).createTextNode(e))},null,e,arguments.length)},append:function(){return this.domManip(arguments,function(e){if(1===this.nodeType||11===this.nodeType||9===this.nodeType){var t=Lt(this,e);t.appendChild(e)}})},prepend:function(){return this.domManip(arguments,function(e){if(1===this.nodeType||11===this.nodeType||9===this.nodeType){var t=Lt(this,e);t.insertBefore(e,t.firstChild)}})},before:function(){return this.domManip(arguments,function(e){this.parentNode&&this.parentNode.insertBefore(e,this)})},after:function(){return this.domManip(arguments,function(e){this.parentNode&&this.parentNode.insertBefore(e,this.nextSibling)})},remove:function(e,t){var n,r=e?x.filter(e,this):this,i=0;for(;null!=(n=r[i]);i++)t||1!==n.nodeType||x.cleanData(Ft(n)),n.parentNode&&(t&&x.contains(n.ownerDocument,n)&&_t(Ft(n,"script")),n.parentNode.removeChild(n));return this},empty:function(){var e,t=0;for(;null!=(e=this[t]);t++){1===e.nodeType&&x.cleanData(Ft(e,!1));while(e.firstChild)e.removeChild(e.firstChild);e.options&&x.nodeName(e,"select")&&(e.options.length=0)}return this},clone:function(e,t){return e=null==e?!1:e,t=null==t?e:t,this.map(function(){return x.clone(this,e,t)})},html:function(e){return x.access(this,function(e){var n=this[0]||{},r=0,i=this.length;if(e===t)return 1===n.nodeType?n.innerHTML.replace(gt,""):t;if(!("string"!=typeof e||Tt.test(e)||!x.support.htmlSerialize&&mt.test(e)||!x.support.leadingWhitespace&&yt.test(e)||At[(bt.exec(e)||["",""])[1].toLowerCase()])){e=e.replace(vt,"<$1></$2>");try{for(;i>r;r++)n=this[r]||{},1===n.nodeType&&(x.cleanData(Ft(n,!1)),n.innerHTML=e);n=0}catch(o){}}n&&this.empty().append(e)},null,e,arguments.length)},replaceWith:function(){var e=x.map(this,function(e){return[e.nextSibling,e.parentNode]}),t=0;return this.domManip(arguments,function(n){var r=e[t++],i=e[t++];i&&(r&&r.parentNode!==i&&(r=this.nextSibling),x(this).remove(),i.insertBefore(n,r))},!0),t?this:this.remove()},detach:function(e){return this.remove(e,!0)},domManip:function(e,t,n){e=d.apply([],e);var r,i,o,a,s,l,u=0,c=this.length,p=this,f=c-1,h=e[0],g=x.isFunction(h);if(g||!(1>=c||"string"!=typeof h||x.support.checkClone)&&Nt.test(h))return this.each(function(r){var i=p.eq(r);g&&(e[0]=h.call(this,r,i.html())),i.domManip(e,t,n)});if(c&&(l=x.buildFragment(e,this[0].ownerDocument,!1,!n&&this),r=l.firstChild,1===l.childNodes.length&&(l=r),r)){for(a=x.map(Ft(l,"script"),Ht),o=a.length;c>u;u++)i=l,u!==f&&(i=x.clone(i,!0,!0),o&&x.merge(a,Ft(i,"script"))),t.call(this[u],i,u);if(o)for(s=a[a.length-1].ownerDocument,x.map(a,qt),u=0;o>u;u++)i=a[u],kt.test(i.type||"")&&!x._data(i,"globalEval")&&x.contains(s,i)&&(i.src?x._evalUrl(i.src):x.globalEval((i.text||i.textContent||i.innerHTML||"").replace(St,"")));l=r=null}return this}});function Lt(e,t){return x.nodeName(e,"table")&&x.nodeName(1===t.nodeType?t:t.firstChild,"tr")?e.getElementsByTagName("tbody")[0]||e.appendChild(e.ownerDocument.createElement("tbody")):e}function Ht(e){return e.type=(null!==x.find.attr(e,"type"))+"/"+e.type,e}function qt(e){var t=Et.exec(e.type);return t?e.type=t[1]:e.removeAttribute("type"),e}function _t(e,t){var n,r=0;for(;null!=(n=e[r]);r++)x._data(n,"globalEval",!t||x._data(t[r],"globalEval"))}function Mt(e,t){if(1===t.nodeType&&x.hasData(e)){var n,r,i,o=x._data(e),a=x._data(t,o),s=o.events;if(s){delete a.handle,a.events={};for(n in s)for(r=0,i=s[n].length;i>r;r++)x.event.add(t,n,s[n][r])}a.data&&(a.data=x.extend({},a.data))}}function Ot(e,t){var n,r,i;if(1===t.nodeType){if(n=t.nodeName.toLowerCase(),!x.support.noCloneEvent&&t[x.expando]){i=x._data(t);for(r in i.events)x.removeEvent(t,r,i.handle);t.removeAttribute(x.expando)}"script"===n&&t.text!==e.text?(Ht(t).text=e.text,qt(t)):"object"===n?(t.parentNode&&(t.outerHTML=e.outerHTML),x.support.html5Clone&&e.innerHTML&&!x.trim(t.innerHTML)&&(t.innerHTML=e.innerHTML)):"input"===n&&Ct.test(e.type)?(t.defaultChecked=t.checked=e.checked,t.value!==e.value&&(t.value=e.value)):"option"===n?t.defaultSelected=t.selected=e.defaultSelected:("input"===n||"textarea"===n)&&(t.defaultValue=e.defaultValue)}}x.each({appendTo:"append",prependTo:"prepend",insertBefore:"before",insertAfter:"after",replaceAll:"replaceWith"},function(e,t){x.fn[e]=function(e){var n,r=0,i=[],o=x(e),a=o.length-1;for(;a>=r;r++)n=r===a?this:this.clone(!0),x(o[r])[t](n),h.apply(i,n.get());return this.pushStack(i)}});function Ft(e,n){var r,o,a=0,s=typeof e.getElementsByTagName!==i?e.getElementsByTagName(n||"*"):typeof e.querySelectorAll!==i?e.querySelectorAll(n||"*"):t;if(!s)for(s=[],r=e.childNodes||e;null!=(o=r[a]);a++)!n||x.nodeName(o,n)?s.push(o):x.merge(s,Ft(o,n));return n===t||n&&x.nodeName(e,n)?x.merge([e],s):s}function Bt(e){Ct.test(e.type)&&(e.defaultChecked=e.checked)}x.extend({clone:function(e,t,n){var r,i,o,a,s,l=x.contains(e.ownerDocument,e);if(x.support.html5Clone||x.isXMLDoc(e)||!mt.test("<"+e.nodeName+">")?o=e.cloneNode(!0):(Dt.innerHTML=e.outerHTML,Dt.removeChild(o=Dt.firstChild)),!(x.support.noCloneEvent&&x.support.noCloneChecked||1!==e.nodeType&&11!==e.nodeType||x.isXMLDoc(e)))for(r=Ft(o),s=Ft(e),a=0;null!=(i=s[a]);++a)r[a]&&Ot(i,r[a]);if(t)if(n)for(s=s||Ft(e),r=r||Ft(o),a=0;null!=(i=s[a]);a++)Mt(i,r[a]);else Mt(e,o);return r=Ft(o,"script"),r.length>0&&_t(r,!l&&Ft(e,"script")),r=s=i=null,o},buildFragment:function(e,t,n,r){var i,o,a,s,l,u,c,p=e.length,f=dt(t),d=[],h=0;for(;p>h;h++)if(o=e[h],o||0===o)if("object"===x.type(o))x.merge(d,o.nodeType?[o]:o);else if(wt.test(o)){s=s||f.appendChild(t.createElement("div")),l=(bt.exec(o)||["",""])[1].toLowerCase(),c=At[l]||At._default,s.innerHTML=c[1]+o.replace(vt,"<$1></$2>")+c[2],i=c[0];while(i--)s=s.lastChild;if(!x.support.leadingWhitespace&&yt.test(o)&&d.push(t.createTextNode(yt.exec(o)[0])),!x.support.tbody){o="table"!==l||xt.test(o)?"<table>"!==c[1]||xt.test(o)?0:s:s.firstChild,i=o&&o.childNodes.length;while(i--)x.nodeName(u=o.childNodes[i],"tbody")&&!u.childNodes.length&&o.removeChild(u)}x.merge(d,s.childNodes),s.textContent="";while(s.firstChild)s.removeChild(s.firstChild);s=f.lastChild}else d.push(t.createTextNode(o));s&&f.removeChild(s),x.support.appendChecked||x.grep(Ft(d,"input"),Bt),h=0;while(o=d[h++])if((!r||-1===x.inArray(o,r))&&(a=x.contains(o.ownerDocument,o),s=Ft(f.appendChild(o),"script"),a&&_t(s),n)){i=0;while(o=s[i++])kt.test(o.type||"")&&n.push(o)}return s=null,f},cleanData:function(e,t){var n,r,o,a,s=0,l=x.expando,u=x.cache,c=x.support.deleteExpando,f=x.event.special;for(;null!=(n=e[s]);s++)if((t||x.acceptData(n))&&(o=n[l],a=o&&u[o])){if(a.events)for(r in a.events)f[r]?x.event.remove(n,r):x.removeEvent(n,r,a.handle);
u[o]&&(delete u[o],c?delete n[l]:typeof n.removeAttribute!==i?n.removeAttribute(l):n[l]=null,p.push(o))}},_evalUrl:function(e){return x.ajax({url:e,type:"GET",dataType:"script",async:!1,global:!1,"throws":!0})}}),x.fn.extend({wrapAll:function(e){if(x.isFunction(e))return this.each(function(t){x(this).wrapAll(e.call(this,t))});if(this[0]){var t=x(e,this[0].ownerDocument).eq(0).clone(!0);this[0].parentNode&&t.insertBefore(this[0]),t.map(function(){var e=this;while(e.firstChild&&1===e.firstChild.nodeType)e=e.firstChild;return e}).append(this)}return this},wrapInner:function(e){return x.isFunction(e)?this.each(function(t){x(this).wrapInner(e.call(this,t))}):this.each(function(){var t=x(this),n=t.contents();n.length?n.wrapAll(e):t.append(e)})},wrap:function(e){var t=x.isFunction(e);return this.each(function(n){x(this).wrapAll(t?e.call(this,n):e)})},unwrap:function(){return this.parent().each(function(){x.nodeName(this,"body")||x(this).replaceWith(this.childNodes)}).end()}});var Pt,Rt,Wt,$t=/alpha\([^)]*\)/i,It=/opacity\s*=\s*([^)]*)/,zt=/^(top|right|bottom|left)$/,Xt=/^(none|table(?!-c[ea]).+)/,Ut=/^margin/,Vt=RegExp("^("+w+")(.*)$","i"),Yt=RegExp("^("+w+")(?!px)[a-z%]+$","i"),Jt=RegExp("^([+-])=("+w+")","i"),Gt={BODY:"block"},Qt={position:"absolute",visibility:"hidden",display:"block"},Kt={letterSpacing:0,fontWeight:400},Zt=["Top","Right","Bottom","Left"],en=["Webkit","O","Moz","ms"];function tn(e,t){if(t in e)return t;var n=t.charAt(0).toUpperCase()+t.slice(1),r=t,i=en.length;while(i--)if(t=en[i]+n,t in e)return t;return r}function nn(e,t){return e=t||e,"none"===x.css(e,"display")||!x.contains(e.ownerDocument,e)}function rn(e,t){var n,r,i,o=[],a=0,s=e.length;for(;s>a;a++)r=e[a],r.style&&(o[a]=x._data(r,"olddisplay"),n=r.style.display,t?(o[a]||"none"!==n||(r.style.display=""),""===r.style.display&&nn(r)&&(o[a]=x._data(r,"olddisplay",ln(r.nodeName)))):o[a]||(i=nn(r),(n&&"none"!==n||!i)&&x._data(r,"olddisplay",i?n:x.css(r,"display"))));for(a=0;s>a;a++)r=e[a],r.style&&(t&&"none"!==r.style.display&&""!==r.style.display||(r.style.display=t?o[a]||"":"none"));return e}x.fn.extend({css:function(e,n){return x.access(this,function(e,n,r){var i,o,a={},s=0;if(x.isArray(n)){for(o=Rt(e),i=n.length;i>s;s++)a[n[s]]=x.css(e,n[s],!1,o);return a}return r!==t?x.style(e,n,r):x.css(e,n)},e,n,arguments.length>1)},show:function(){return rn(this,!0)},hide:function(){return rn(this)},toggle:function(e){var t="boolean"==typeof e;return this.each(function(){(t?e:nn(this))?x(this).show():x(this).hide()})}}),x.extend({cssHooks:{opacity:{get:function(e,t){if(t){var n=Wt(e,"opacity");return""===n?"1":n}}}},cssNumber:{columnCount:!0,fillOpacity:!0,fontWeight:!0,lineHeight:!0,opacity:!0,orphans:!0,widows:!0,zIndex:!0,zoom:!0},cssProps:{"float":x.support.cssFloat?"cssFloat":"styleFloat"},style:function(e,n,r,i){if(e&&3!==e.nodeType&&8!==e.nodeType&&e.style){var o,a,s,l=x.camelCase(n),u=e.style;if(n=x.cssProps[l]||(x.cssProps[l]=tn(u,l)),s=x.cssHooks[n]||x.cssHooks[l],r===t)return s&&"get"in s&&(o=s.get(e,!1,i))!==t?o:u[n];if(a=typeof r,"string"===a&&(o=Jt.exec(r))&&(r=(o[1]+1)*o[2]+parseFloat(x.css(e,n)),a="number"),!(null==r||"number"===a&&isNaN(r)||("number"!==a||x.cssNumber[l]||(r+="px"),x.support.clearCloneStyle||""!==r||0!==n.indexOf("background")||(u[n]="inherit"),s&&"set"in s&&(r=s.set(e,r,i))===t)))try{u[n]=r}catch(c){}}},css:function(e,n,r,i){var o,a,s,l=x.camelCase(n);return n=x.cssProps[l]||(x.cssProps[l]=tn(e.style,l)),s=x.cssHooks[n]||x.cssHooks[l],s&&"get"in s&&(a=s.get(e,!0,r)),a===t&&(a=Wt(e,n,i)),"normal"===a&&n in Kt&&(a=Kt[n]),""===r||r?(o=parseFloat(a),r===!0||x.isNumeric(o)?o||0:a):a}}),e.getComputedStyle?(Rt=function(t){return e.getComputedStyle(t,null)},Wt=function(e,n,r){var i,o,a,s=r||Rt(e),l=s?s.getPropertyValue(n)||s[n]:t,u=e.style;return s&&(""!==l||x.contains(e.ownerDocument,e)||(l=x.style(e,n)),Yt.test(l)&&Ut.test(n)&&(i=u.width,o=u.minWidth,a=u.maxWidth,u.minWidth=u.maxWidth=u.width=l,l=s.width,u.width=i,u.minWidth=o,u.maxWidth=a)),l}):a.documentElement.currentStyle&&(Rt=function(e){return e.currentStyle},Wt=function(e,n,r){var i,o,a,s=r||Rt(e),l=s?s[n]:t,u=e.style;return null==l&&u&&u[n]&&(l=u[n]),Yt.test(l)&&!zt.test(n)&&(i=u.left,o=e.runtimeStyle,a=o&&o.left,a&&(o.left=e.currentStyle.left),u.left="fontSize"===n?"1em":l,l=u.pixelLeft+"px",u.left=i,a&&(o.left=a)),""===l?"auto":l});function on(e,t,n){var r=Vt.exec(t);return r?Math.max(0,r[1]-(n||0))+(r[2]||"px"):t}function an(e,t,n,r,i){var o=n===(r?"border":"content")?4:"width"===t?1:0,a=0;for(;4>o;o+=2)"margin"===n&&(a+=x.css(e,n+Zt[o],!0,i)),r?("content"===n&&(a-=x.css(e,"padding"+Zt[o],!0,i)),"margin"!==n&&(a-=x.css(e,"border"+Zt[o]+"Width",!0,i))):(a+=x.css(e,"padding"+Zt[o],!0,i),"padding"!==n&&(a+=x.css(e,"border"+Zt[o]+"Width",!0,i)));return a}function sn(e,t,n){var r=!0,i="width"===t?e.offsetWidth:e.offsetHeight,o=Rt(e),a=x.support.boxSizing&&"border-box"===x.css(e,"boxSizing",!1,o);if(0>=i||null==i){if(i=Wt(e,t,o),(0>i||null==i)&&(i=e.style[t]),Yt.test(i))return i;r=a&&(x.support.boxSizingReliable||i===e.style[t]),i=parseFloat(i)||0}return i+an(e,t,n||(a?"border":"content"),r,o)+"px"}function ln(e){var t=a,n=Gt[e];return n||(n=un(e,t),"none"!==n&&n||(Pt=(Pt||x("<iframe frameborder='0' width='0' height='0'/>").css("cssText","display:block !important")).appendTo(t.documentElement),t=(Pt[0].contentWindow||Pt[0].contentDocument).document,t.write("<!doctype html><html><body>"),t.close(),n=un(e,t),Pt.detach()),Gt[e]=n),n}function un(e,t){var n=x(t.createElement(e)).appendTo(t.body),r=x.css(n[0],"display");return n.remove(),r}x.each(["height","width"],function(e,n){x.cssHooks[n]={get:function(e,r,i){return r?0===e.offsetWidth&&Xt.test(x.css(e,"display"))?x.swap(e,Qt,function(){return sn(e,n,i)}):sn(e,n,i):t},set:function(e,t,r){var i=r&&Rt(e);return on(e,t,r?an(e,n,r,x.support.boxSizing&&"border-box"===x.css(e,"boxSizing",!1,i),i):0)}}}),x.support.opacity||(x.cssHooks.opacity={get:function(e,t){return It.test((t&&e.currentStyle?e.currentStyle.filter:e.style.filter)||"")?.01*parseFloat(RegExp.$1)+"":t?"1":""},set:function(e,t){var n=e.style,r=e.currentStyle,i=x.isNumeric(t)?"alpha(opacity="+100*t+")":"",o=r&&r.filter||n.filter||"";n.zoom=1,(t>=1||""===t)&&""===x.trim(o.replace($t,""))&&n.removeAttribute&&(n.removeAttribute("filter"),""===t||r&&!r.filter)||(n.filter=$t.test(o)?o.replace($t,i):o+" "+i)}}),x(function(){x.support.reliableMarginRight||(x.cssHooks.marginRight={get:function(e,n){return n?x.swap(e,{display:"inline-block"},Wt,[e,"marginRight"]):t}}),!x.support.pixelPosition&&x.fn.position&&x.each(["top","left"],function(e,n){x.cssHooks[n]={get:function(e,r){return r?(r=Wt(e,n),Yt.test(r)?x(e).position()[n]+"px":r):t}}})}),x.expr&&x.expr.filters&&(x.expr.filters.hidden=function(e){return 0>=e.offsetWidth&&0>=e.offsetHeight||!x.support.reliableHiddenOffsets&&"none"===(e.style&&e.style.display||x.css(e,"display"))},x.expr.filters.visible=function(e){return!x.expr.filters.hidden(e)}),x.each({margin:"",padding:"",border:"Width"},function(e,t){x.cssHooks[e+t]={expand:function(n){var r=0,i={},o="string"==typeof n?n.split(" "):[n];for(;4>r;r++)i[e+Zt[r]+t]=o[r]||o[r-2]||o[0];return i}},Ut.test(e)||(x.cssHooks[e+t].set=on)});var cn=/%20/g,pn=/\[\]$/,fn=/\r?\n/g,dn=/^(?:submit|button|image|reset|file)$/i,hn=/^(?:input|select|textarea|keygen)/i;x.fn.extend({serialize:function(){return x.param(this.serializeArray())},serializeArray:function(){return this.map(function(){var e=x.prop(this,"elements");return e?x.makeArray(e):this}).filter(function(){var e=this.type;return this.name&&!x(this).is(":disabled")&&hn.test(this.nodeName)&&!dn.test(e)&&(this.checked||!Ct.test(e))}).map(function(e,t){var n=x(this).val();return null==n?null:x.isArray(n)?x.map(n,function(e){return{name:t.name,value:e.replace(fn,"\r\n")}}):{name:t.name,value:n.replace(fn,"\r\n")}}).get()}}),x.param=function(e,n){var r,i=[],o=function(e,t){t=x.isFunction(t)?t():null==t?"":t,i[i.length]=encodeURIComponent(e)+"="+encodeURIComponent(t)};if(n===t&&(n=x.ajaxSettings&&x.ajaxSettings.traditional),x.isArray(e)||e.jquery&&!x.isPlainObject(e))x.each(e,function(){o(this.name,this.value)});else for(r in e)gn(r,e[r],n,o);return i.join("&").replace(cn,"+")};function gn(e,t,n,r){var i;if(x.isArray(t))x.each(t,function(t,i){n||pn.test(e)?r(e,i):gn(e+"["+("object"==typeof i?t:"")+"]",i,n,r)});else if(n||"object"!==x.type(t))r(e,t);else for(i in t)gn(e+"["+i+"]",t[i],n,r)}x.each("blur focus focusin focusout load resize scroll unload click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave change select submit keydown keypress keyup error contextmenu".split(" "),function(e,t){x.fn[t]=function(e,n){return arguments.length>0?this.on(t,null,e,n):this.trigger(t)}}),x.fn.extend({hover:function(e,t){return this.mouseenter(e).mouseleave(t||e)},bind:function(e,t,n){return this.on(e,null,t,n)},unbind:function(e,t){return this.off(e,null,t)},delegate:function(e,t,n,r){return this.on(t,e,n,r)},undelegate:function(e,t,n){return 1===arguments.length?this.off(e,"**"):this.off(t,e||"**",n)}});var mn,yn,vn=x.now(),bn=/\?/,xn=/#.*$/,wn=/([?&])_=[^&]*/,Tn=/^(.*?):[ \t]*([^\r\n]*)\r?$/gm,Cn=/^(?:about|app|app-storage|.+-extension|file|res|widget):$/,Nn=/^(?:GET|HEAD)$/,kn=/^\/\//,En=/^([\w.+-]+:)(?:\/\/([^\/?#:]*)(?::(\d+)|)|)/,Sn=x.fn.load,An={},jn={},Dn="*/".concat("*");try{yn=o.href}catch(Ln){yn=a.createElement("a"),yn.href="",yn=yn.href}mn=En.exec(yn.toLowerCase())||[];function Hn(e){return function(t,n){"string"!=typeof t&&(n=t,t="*");var r,i=0,o=t.toLowerCase().match(T)||[];if(x.isFunction(n))while(r=o[i++])"+"===r[0]?(r=r.slice(1)||"*",(e[r]=e[r]||[]).unshift(n)):(e[r]=e[r]||[]).push(n)}}function qn(e,n,r,i){var o={},a=e===jn;function s(l){var u;return o[l]=!0,x.each(e[l]||[],function(e,l){var c=l(n,r,i);return"string"!=typeof c||a||o[c]?a?!(u=c):t:(n.dataTypes.unshift(c),s(c),!1)}),u}return s(n.dataTypes[0])||!o["*"]&&s("*")}function _n(e,n){var r,i,o=x.ajaxSettings.flatOptions||{};for(i in n)n[i]!==t&&((o[i]?e:r||(r={}))[i]=n[i]);return r&&x.extend(!0,e,r),e}x.fn.load=function(e,n,r){if("string"!=typeof e&&Sn)return Sn.apply(this,arguments);var i,o,a,s=this,l=e.indexOf(" ");return l>=0&&(i=e.slice(l,e.length),e=e.slice(0,l)),x.isFunction(n)?(r=n,n=t):n&&"object"==typeof n&&(a="POST"),s.length>0&&x.ajax({url:e,type:a,dataType:"html",data:n}).done(function(e){o=arguments,s.html(i?x("<div>").append(x.parseHTML(e)).find(i):e)}).complete(r&&function(e,t){s.each(r,o||[e.responseText,t,e])}),this},x.each(["ajaxStart","ajaxStop","ajaxComplete","ajaxError","ajaxSuccess","ajaxSend"],function(e,t){x.fn[t]=function(e){return this.on(t,e)}}),x.extend({active:0,lastModified:{},etag:{},ajaxSettings:{url:yn,type:"GET",isLocal:Cn.test(mn[1]),global:!0,processData:!0,async:!0,contentType:"application/x-www-form-urlencoded; charset=UTF-8",accepts:{"*":Dn,text:"text/plain",html:"text/html",xml:"application/xml, text/xml",json:"application/json, text/javascript"},contents:{xml:/xml/,html:/html/,json:/json/},responseFields:{xml:"responseXML",text:"responseText",json:"responseJSON"},converters:{"* text":String,"text html":!0,"text json":x.parseJSON,"text xml":x.parseXML},flatOptions:{url:!0,context:!0}},ajaxSetup:function(e,t){return t?_n(_n(e,x.ajaxSettings),t):_n(x.ajaxSettings,e)},ajaxPrefilter:Hn(An),ajaxTransport:Hn(jn),ajax:function(e,n){"object"==typeof e&&(n=e,e=t),n=n||{};var r,i,o,a,s,l,u,c,p=x.ajaxSetup({},n),f=p.context||p,d=p.context&&(f.nodeType||f.jquery)?x(f):x.event,h=x.Deferred(),g=x.Callbacks("once memory"),m=p.statusCode||{},y={},v={},b=0,w="canceled",C={readyState:0,getResponseHeader:function(e){var t;if(2===b){if(!c){c={};while(t=Tn.exec(a))c[t[1].toLowerCase()]=t[2]}t=c[e.toLowerCase()]}return null==t?null:t},getAllResponseHeaders:function(){return 2===b?a:null},setRequestHeader:function(e,t){var n=e.toLowerCase();return b||(e=v[n]=v[n]||e,y[e]=t),this},overrideMimeType:function(e){return b||(p.mimeType=e),this},statusCode:function(e){var t;if(e)if(2>b)for(t in e)m[t]=[m[t],e[t]];else C.always(e[C.status]);return this},abort:function(e){var t=e||w;return u&&u.abort(t),k(0,t),this}};if(h.promise(C).complete=g.add,C.success=C.done,C.error=C.fail,p.url=((e||p.url||yn)+"").replace(xn,"").replace(kn,mn[1]+"//"),p.type=n.method||n.type||p.method||p.type,p.dataTypes=x.trim(p.dataType||"*").toLowerCase().match(T)||[""],null==p.crossDomain&&(r=En.exec(p.url.toLowerCase()),p.crossDomain=!(!r||r[1]===mn[1]&&r[2]===mn[2]&&(r[3]||("http:"===r[1]?"80":"443"))===(mn[3]||("http:"===mn[1]?"80":"443")))),p.data&&p.processData&&"string"!=typeof p.data&&(p.data=x.param(p.data,p.traditional)),qn(An,p,n,C),2===b)return C;l=p.global,l&&0===x.active++&&x.event.trigger("ajaxStart"),p.type=p.type.toUpperCase(),p.hasContent=!Nn.test(p.type),o=p.url,p.hasContent||(p.data&&(o=p.url+=(bn.test(o)?"&":"?")+p.data,delete p.data),p.cache===!1&&(p.url=wn.test(o)?o.replace(wn,"$1_="+vn++):o+(bn.test(o)?"&":"?")+"_="+vn++)),p.ifModified&&(x.lastModified[o]&&C.setRequestHeader("If-Modified-Since",x.lastModified[o]),x.etag[o]&&C.setRequestHeader("If-None-Match",x.etag[o])),(p.data&&p.hasContent&&p.contentType!==!1||n.contentType)&&C.setRequestHeader("Content-Type",p.contentType),C.setRequestHeader("Accept",p.dataTypes[0]&&p.accepts[p.dataTypes[0]]?p.accepts[p.dataTypes[0]]+("*"!==p.dataTypes[0]?", "+Dn+"; q=0.01":""):p.accepts["*"]);for(i in p.headers)C.setRequestHeader(i,p.headers[i]);if(p.beforeSend&&(p.beforeSend.call(f,C,p)===!1||2===b))return C.abort();w="abort";for(i in{success:1,error:1,complete:1})C[i](p[i]);if(u=qn(jn,p,n,C)){C.readyState=1,l&&d.trigger("ajaxSend",[C,p]),p.async&&p.timeout>0&&(s=setTimeout(function(){C.abort("timeout")},p.timeout));try{b=1,u.send(y,k)}catch(N){if(!(2>b))throw N;k(-1,N)}}else k(-1,"No Transport");function k(e,n,r,i){var c,y,v,w,T,N=n;2!==b&&(b=2,s&&clearTimeout(s),u=t,a=i||"",C.readyState=e>0?4:0,c=e>=200&&300>e||304===e,r&&(w=Mn(p,C,r)),w=On(p,w,C,c),c?(p.ifModified&&(T=C.getResponseHeader("Last-Modified"),T&&(x.lastModified[o]=T),T=C.getResponseHeader("etag"),T&&(x.etag[o]=T)),204===e||"HEAD"===p.type?N="nocontent":304===e?N="notmodified":(N=w.state,y=w.data,v=w.error,c=!v)):(v=N,(e||!N)&&(N="error",0>e&&(e=0))),C.status=e,C.statusText=(n||N)+"",c?h.resolveWith(f,[y,N,C]):h.rejectWith(f,[C,N,v]),C.statusCode(m),m=t,l&&d.trigger(c?"ajaxSuccess":"ajaxError",[C,p,c?y:v]),g.fireWith(f,[C,N]),l&&(d.trigger("ajaxComplete",[C,p]),--x.active||x.event.trigger("ajaxStop")))}return C},getJSON:function(e,t,n){return x.get(e,t,n,"json")},getScript:function(e,n){return x.get(e,t,n,"script")}}),x.each(["get","post"],function(e,n){x[n]=function(e,r,i,o){return x.isFunction(r)&&(o=o||i,i=r,r=t),x.ajax({url:e,type:n,dataType:o,data:r,success:i})}});function Mn(e,n,r){var i,o,a,s,l=e.contents,u=e.dataTypes;while("*"===u[0])u.shift(),o===t&&(o=e.mimeType||n.getResponseHeader("Content-Type"));if(o)for(s in l)if(l[s]&&l[s].test(o)){u.unshift(s);break}if(u[0]in r)a=u[0];else{for(s in r){if(!u[0]||e.converters[s+" "+u[0]]){a=s;break}i||(i=s)}a=a||i}return a?(a!==u[0]&&u.unshift(a),r[a]):t}function On(e,t,n,r){var i,o,a,s,l,u={},c=e.dataTypes.slice();if(c[1])for(a in e.converters)u[a.toLowerCase()]=e.converters[a];o=c.shift();while(o)if(e.responseFields[o]&&(n[e.responseFields[o]]=t),!l&&r&&e.dataFilter&&(t=e.dataFilter(t,e.dataType)),l=o,o=c.shift())if("*"===o)o=l;else if("*"!==l&&l!==o){if(a=u[l+" "+o]||u["* "+o],!a)for(i in u)if(s=i.split(" "),s[1]===o&&(a=u[l+" "+s[0]]||u["* "+s[0]])){a===!0?a=u[i]:u[i]!==!0&&(o=s[0],c.unshift(s[1]));break}if(a!==!0)if(a&&e["throws"])t=a(t);else try{t=a(t)}catch(p){return{state:"parsererror",error:a?p:"No conversion from "+l+" to "+o}}}return{state:"success",data:t}}x.ajaxSetup({accepts:{script:"text/javascript, application/javascript, application/ecmascript, application/x-ecmascript"},contents:{script:/(?:java|ecma)script/},converters:{"text script":function(e){return x.globalEval(e),e}}}),x.ajaxPrefilter("script",function(e){e.cache===t&&(e.cache=!1),e.crossDomain&&(e.type="GET",e.global=!1)}),x.ajaxTransport("script",function(e){if(e.crossDomain){var n,r=a.head||x("head")[0]||a.documentElement;return{send:function(t,i){n=a.createElement("script"),n.async=!0,e.scriptCharset&&(n.charset=e.scriptCharset),n.src=e.url,n.onload=n.onreadystatechange=function(e,t){(t||!n.readyState||/loaded|complete/.test(n.readyState))&&(n.onload=n.onreadystatechange=null,n.parentNode&&n.parentNode.removeChild(n),n=null,t||i(200,"success"))},r.insertBefore(n,r.firstChild)},abort:function(){n&&n.onload(t,!0)}}}});var Fn=[],Bn=/(=)\?(?=&|$)|\?\?/;x.ajaxSetup({jsonp:"callback",jsonpCallback:function(){var e=Fn.pop()||x.expando+"_"+vn++;return this[e]=!0,e}}),x.ajaxPrefilter("json jsonp",function(n,r,i){var o,a,s,l=n.jsonp!==!1&&(Bn.test(n.url)?"url":"string"==typeof n.data&&!(n.contentType||"").indexOf("application/x-www-form-urlencoded")&&Bn.test(n.data)&&"data");return l||"jsonp"===n.dataTypes[0]?(o=n.jsonpCallback=x.isFunction(n.jsonpCallback)?n.jsonpCallback():n.jsonpCallback,l?n[l]=n[l].replace(Bn,"$1"+o):n.jsonp!==!1&&(n.url+=(bn.test(n.url)?"&":"?")+n.jsonp+"="+o),n.converters["script json"]=function(){return s||x.error(o+" was not called"),s[0]},n.dataTypes[0]="json",a=e[o],e[o]=function(){s=arguments},i.always(function(){e[o]=a,n[o]&&(n.jsonpCallback=r.jsonpCallback,Fn.push(o)),s&&x.isFunction(a)&&a(s[0]),s=a=t}),"script"):t});var Pn,Rn,Wn=0,$n=e.ActiveXObject&&function(){var e;for(e in Pn)Pn[e](t,!0)};function In(){try{return new e.XMLHttpRequest}catch(t){}}function zn(){try{return new e.ActiveXObject("Microsoft.XMLHTTP")}catch(t){}}x.ajaxSettings.xhr=e.ActiveXObject?function(){return!this.isLocal&&In()||zn()}:In,Rn=x.ajaxSettings.xhr(),x.support.cors=!!Rn&&"withCredentials"in Rn,Rn=x.support.ajax=!!Rn,Rn&&x.ajaxTransport(function(n){if(!n.crossDomain||x.support.cors){var r;return{send:function(i,o){var a,s,l=n.xhr();if(n.username?l.open(n.type,n.url,n.async,n.username,n.password):l.open(n.type,n.url,n.async),n.xhrFields)for(s in n.xhrFields)l[s]=n.xhrFields[s];n.mimeType&&l.overrideMimeType&&l.overrideMimeType(n.mimeType),n.crossDomain||i["X-Requested-With"]||(i["X-Requested-With"]="XMLHttpRequest");try{for(s in i)l.setRequestHeader(s,i[s])}catch(u){}l.send(n.hasContent&&n.data||null),r=function(e,i){var s,u,c,p;try{if(r&&(i||4===l.readyState))if(r=t,a&&(l.onreadystatechange=x.noop,$n&&delete Pn[a]),i)4!==l.readyState&&l.abort();else{p={},s=l.status,u=l.getAllResponseHeaders(),"string"==typeof l.responseText&&(p.text=l.responseText);try{c=l.statusText}catch(f){c=""}s||!n.isLocal||n.crossDomain?1223===s&&(s=204):s=p.text?200:404}}catch(d){i||o(-1,d)}p&&o(s,c,p,u)},n.async?4===l.readyState?setTimeout(r):(a=++Wn,$n&&(Pn||(Pn={},x(e).unload($n)),Pn[a]=r),l.onreadystatechange=r):r()},abort:function(){r&&r(t,!0)}}}});var Xn,Un,Vn=/^(?:toggle|show|hide)$/,Yn=RegExp("^(?:([+-])=|)("+w+")([a-z%]*)$","i"),Jn=/queueHooks$/,Gn=[nr],Qn={"*":[function(e,t){var n=this.createTween(e,t),r=n.cur(),i=Yn.exec(t),o=i&&i[3]||(x.cssNumber[e]?"":"px"),a=(x.cssNumber[e]||"px"!==o&&+r)&&Yn.exec(x.css(n.elem,e)),s=1,l=20;if(a&&a[3]!==o){o=o||a[3],i=i||[],a=+r||1;do s=s||".5",a/=s,x.style(n.elem,e,a+o);while(s!==(s=n.cur()/r)&&1!==s&&--l)}return i&&(a=n.start=+a||+r||0,n.unit=o,n.end=i[1]?a+(i[1]+1)*i[2]:+i[2]),n}]};function Kn(){return setTimeout(function(){Xn=t}),Xn=x.now()}function Zn(e,t,n){var r,i=(Qn[t]||[]).concat(Qn["*"]),o=0,a=i.length;for(;a>o;o++)if(r=i[o].call(n,t,e))return r}function er(e,t,n){var r,i,o=0,a=Gn.length,s=x.Deferred().always(function(){delete l.elem}),l=function(){if(i)return!1;var t=Xn||Kn(),n=Math.max(0,u.startTime+u.duration-t),r=n/u.duration||0,o=1-r,a=0,l=u.tweens.length;for(;l>a;a++)u.tweens[a].run(o);return s.notifyWith(e,[u,o,n]),1>o&&l?n:(s.resolveWith(e,[u]),!1)},u=s.promise({elem:e,props:x.extend({},t),opts:x.extend(!0,{specialEasing:{}},n),originalProperties:t,originalOptions:n,startTime:Xn||Kn(),duration:n.duration,tweens:[],createTween:function(t,n){var r=x.Tween(e,u.opts,t,n,u.opts.specialEasing[t]||u.opts.easing);return u.tweens.push(r),r},stop:function(t){var n=0,r=t?u.tweens.length:0;if(i)return this;for(i=!0;r>n;n++)u.tweens[n].run(1);return t?s.resolveWith(e,[u,t]):s.rejectWith(e,[u,t]),this}}),c=u.props;for(tr(c,u.opts.specialEasing);a>o;o++)if(r=Gn[o].call(u,e,c,u.opts))return r;return x.map(c,Zn,u),x.isFunction(u.opts.start)&&u.opts.start.call(e,u),x.fx.timer(x.extend(l,{elem:e,anim:u,queue:u.opts.queue})),u.progress(u.opts.progress).done(u.opts.done,u.opts.complete).fail(u.opts.fail).always(u.opts.always)}function tr(e,t){var n,r,i,o,a;for(n in e)if(r=x.camelCase(n),i=t[r],o=e[n],x.isArray(o)&&(i=o[1],o=e[n]=o[0]),n!==r&&(e[r]=o,delete e[n]),a=x.cssHooks[r],a&&"expand"in a){o=a.expand(o),delete e[r];for(n in o)n in e||(e[n]=o[n],t[n]=i)}else t[r]=i}x.Animation=x.extend(er,{tweener:function(e,t){x.isFunction(e)?(t=e,e=["*"]):e=e.split(" ");var n,r=0,i=e.length;for(;i>r;r++)n=e[r],Qn[n]=Qn[n]||[],Qn[n].unshift(t)},prefilter:function(e,t){t?Gn.unshift(e):Gn.push(e)}});function nr(e,t,n){var r,i,o,a,s,l,u=this,c={},p=e.style,f=e.nodeType&&nn(e),d=x._data(e,"fxshow");n.queue||(s=x._queueHooks(e,"fx"),null==s.unqueued&&(s.unqueued=0,l=s.empty.fire,s.empty.fire=function(){s.unqueued||l()}),s.unqueued++,u.always(function(){u.always(function(){s.unqueued--,x.queue(e,"fx").length||s.empty.fire()})})),1===e.nodeType&&("height"in t||"width"in t)&&(n.overflow=[p.overflow,p.overflowX,p.overflowY],"inline"===x.css(e,"display")&&"none"===x.css(e,"float")&&(x.support.inlineBlockNeedsLayout&&"inline"!==ln(e.nodeName)?p.zoom=1:p.display="inline-block")),n.overflow&&(p.overflow="hidden",x.support.shrinkWrapBlocks||u.always(function(){p.overflow=n.overflow[0],p.overflowX=n.overflow[1],p.overflowY=n.overflow[2]}));for(r in t)if(i=t[r],Vn.exec(i)){if(delete t[r],o=o||"toggle"===i,i===(f?"hide":"show"))continue;c[r]=d&&d[r]||x.style(e,r)}if(!x.isEmptyObject(c)){d?"hidden"in d&&(f=d.hidden):d=x._data(e,"fxshow",{}),o&&(d.hidden=!f),f?x(e).show():u.done(function(){x(e).hide()}),u.done(function(){var t;x._removeData(e,"fxshow");for(t in c)x.style(e,t,c[t])});for(r in c)a=Zn(f?d[r]:0,r,u),r in d||(d[r]=a.start,f&&(a.end=a.start,a.start="width"===r||"height"===r?1:0))}}function rr(e,t,n,r,i){return new rr.prototype.init(e,t,n,r,i)}x.Tween=rr,rr.prototype={constructor:rr,init:function(e,t,n,r,i,o){this.elem=e,this.prop=n,this.easing=i||"swing",this.options=t,this.start=this.now=this.cur(),this.end=r,this.unit=o||(x.cssNumber[n]?"":"px")},cur:function(){var e=rr.propHooks[this.prop];return e&&e.get?e.get(this):rr.propHooks._default.get(this)},run:function(e){var t,n=rr.propHooks[this.prop];return this.pos=t=this.options.duration?x.easing[this.easing](e,this.options.duration*e,0,1,this.options.duration):e,this.now=(this.end-this.start)*t+this.start,this.options.step&&this.options.step.call(this.elem,this.now,this),n&&n.set?n.set(this):rr.propHooks._default.set(this),this}},rr.prototype.init.prototype=rr.prototype,rr.propHooks={_default:{get:function(e){var t;return null==e.elem[e.prop]||e.elem.style&&null!=e.elem.style[e.prop]?(t=x.css(e.elem,e.prop,""),t&&"auto"!==t?t:0):e.elem[e.prop]},set:function(e){x.fx.step[e.prop]?x.fx.step[e.prop](e):e.elem.style&&(null!=e.elem.style[x.cssProps[e.prop]]||x.cssHooks[e.prop])?x.style(e.elem,e.prop,e.now+e.unit):e.elem[e.prop]=e.now}}},rr.propHooks.scrollTop=rr.propHooks.scrollLeft={set:function(e){e.elem.nodeType&&e.elem.parentNode&&(e.elem[e.prop]=e.now)}},x.each(["toggle","show","hide"],function(e,t){var n=x.fn[t];x.fn[t]=function(e,r,i){return null==e||"boolean"==typeof e?n.apply(this,arguments):this.animate(ir(t,!0),e,r,i)}}),x.fn.extend({fadeTo:function(e,t,n,r){return this.filter(nn).css("opacity",0).show().end().animate({opacity:t},e,n,r)},animate:function(e,t,n,r){var i=x.isEmptyObject(e),o=x.speed(t,n,r),a=function(){var t=er(this,x.extend({},e),o);(i||x._data(this,"finish"))&&t.stop(!0)};return a.finish=a,i||o.queue===!1?this.each(a):this.queue(o.queue,a)},stop:function(e,n,r){var i=function(e){var t=e.stop;delete e.stop,t(r)};return"string"!=typeof e&&(r=n,n=e,e=t),n&&e!==!1&&this.queue(e||"fx",[]),this.each(function(){var t=!0,n=null!=e&&e+"queueHooks",o=x.timers,a=x._data(this);if(n)a[n]&&a[n].stop&&i(a[n]);else for(n in a)a[n]&&a[n].stop&&Jn.test(n)&&i(a[n]);for(n=o.length;n--;)o[n].elem!==this||null!=e&&o[n].queue!==e||(o[n].anim.stop(r),t=!1,o.splice(n,1));(t||!r)&&x.dequeue(this,e)})},finish:function(e){return e!==!1&&(e=e||"fx"),this.each(function(){var t,n=x._data(this),r=n[e+"queue"],i=n[e+"queueHooks"],o=x.timers,a=r?r.length:0;for(n.finish=!0,x.queue(this,e,[]),i&&i.stop&&i.stop.call(this,!0),t=o.length;t--;)o[t].elem===this&&o[t].queue===e&&(o[t].anim.stop(!0),o.splice(t,1));for(t=0;a>t;t++)r[t]&&r[t].finish&&r[t].finish.call(this);delete n.finish})}});function ir(e,t){var n,r={height:e},i=0;for(t=t?1:0;4>i;i+=2-t)n=Zt[i],r["margin"+n]=r["padding"+n]=e;return t&&(r.opacity=r.width=e),r}x.each({slideDown:ir("show"),slideUp:ir("hide"),slideToggle:ir("toggle"),fadeIn:{opacity:"show"},fadeOut:{opacity:"hide"},fadeToggle:{opacity:"toggle"}},function(e,t){x.fn[e]=function(e,n,r){return this.animate(t,e,n,r)}}),x.speed=function(e,t,n){var r=e&&"object"==typeof e?x.extend({},e):{complete:n||!n&&t||x.isFunction(e)&&e,duration:e,easing:n&&t||t&&!x.isFunction(t)&&t};return r.duration=x.fx.off?0:"number"==typeof r.duration?r.duration:r.duration in x.fx.speeds?x.fx.speeds[r.duration]:x.fx.speeds._default,(null==r.queue||r.queue===!0)&&(r.queue="fx"),r.old=r.complete,r.complete=function(){x.isFunction(r.old)&&r.old.call(this),r.queue&&x.dequeue(this,r.queue)},r},x.easing={linear:function(e){return e},swing:function(e){return.5-Math.cos(e*Math.PI)/2}},x.timers=[],x.fx=rr.prototype.init,x.fx.tick=function(){var e,n=x.timers,r=0;for(Xn=x.now();n.length>r;r++)e=n[r],e()||n[r]!==e||n.splice(r--,1);n.length||x.fx.stop(),Xn=t},x.fx.timer=function(e){e()&&x.timers.push(e)&&x.fx.start()},x.fx.interval=13,x.fx.start=function(){Un||(Un=setInterval(x.fx.tick,x.fx.interval))},x.fx.stop=function(){clearInterval(Un),Un=null},x.fx.speeds={slow:600,fast:200,_default:400},x.fx.step={},x.expr&&x.expr.filters&&(x.expr.filters.animated=function(e){return x.grep(x.timers,function(t){return e===t.elem}).length}),x.fn.offset=function(e){if(arguments.length)return e===t?this:this.each(function(t){x.offset.setOffset(this,e,t)});var n,r,o={top:0,left:0},a=this[0],s=a&&a.ownerDocument;if(s)return n=s.documentElement,x.contains(n,a)?(typeof a.getBoundingClientRect!==i&&(o=a.getBoundingClientRect()),r=or(s),{top:o.top+(r.pageYOffset||n.scrollTop)-(n.clientTop||0),left:o.left+(r.pageXOffset||n.scrollLeft)-(n.clientLeft||0)}):o},x.offset={setOffset:function(e,t,n){var r=x.css(e,"position");"static"===r&&(e.style.position="relative");var i=x(e),o=i.offset(),a=x.css(e,"top"),s=x.css(e,"left"),l=("absolute"===r||"fixed"===r)&&x.inArray("auto",[a,s])>-1,u={},c={},p,f;l?(c=i.position(),p=c.top,f=c.left):(p=parseFloat(a)||0,f=parseFloat(s)||0),x.isFunction(t)&&(t=t.call(e,n,o)),null!=t.top&&(u.top=t.top-o.top+p),null!=t.left&&(u.left=t.left-o.left+f),"using"in t?t.using.call(e,u):i.css(u)}},x.fn.extend({position:function(){if(this[0]){var e,t,n={top:0,left:0},r=this[0];return"fixed"===x.css(r,"position")?t=r.getBoundingClientRect():(e=this.offsetParent(),t=this.offset(),x.nodeName(e[0],"html")||(n=e.offset()),n.top+=x.css(e[0],"borderTopWidth",!0),n.left+=x.css(e[0],"borderLeftWidth",!0)),{top:t.top-n.top-x.css(r,"marginTop",!0),left:t.left-n.left-x.css(r,"marginLeft",!0)}}},offsetParent:function(){return this.map(function(){var e=this.offsetParent||s;while(e&&!x.nodeName(e,"html")&&"static"===x.css(e,"position"))e=e.offsetParent;return e||s})}}),x.each({scrollLeft:"pageXOffset",scrollTop:"pageYOffset"},function(e,n){var r=/Y/.test(n);x.fn[e]=function(i){return x.access(this,function(e,i,o){var a=or(e);return o===t?a?n in a?a[n]:a.document.documentElement[i]:e[i]:(a?a.scrollTo(r?x(a).scrollLeft():o,r?o:x(a).scrollTop()):e[i]=o,t)},e,i,arguments.length,null)}});function or(e){return x.isWindow(e)?e:9===e.nodeType?e.defaultView||e.parentWindow:!1}x.each({Height:"height",Width:"width"},function(e,n){x.each({padding:"inner"+e,content:n,"":"outer"+e},function(r,i){x.fn[i]=function(i,o){var a=arguments.length&&(r||"boolean"!=typeof i),s=r||(i===!0||o===!0?"margin":"border");return x.access(this,function(n,r,i){var o;return x.isWindow(n)?n.document.documentElement["client"+e]:9===n.nodeType?(o=n.documentElement,Math.max(n.body["scroll"+e],o["scroll"+e],n.body["offset"+e],o["offset"+e],o["client"+e])):i===t?x.css(n,r,s):x.style(n,r,i,s)},n,a?i:t,a,null)}})}),x.fn.size=function(){return this.length},x.fn.andSelf=x.fn.addBack,"object"==typeof module&&module&&"object"==typeof module.exports?module.exports=x:(e.jQuery=e.$=x,"function"==typeof define&&define.amd&&define("jquery",[],function(){return x}))})(window);

/**
 * Prism: Lightweight, robust, elegant syntax highlighting
 * MIT license http://www.opensource.org/licenses/mit-license.php/
 * @author Lea Verou http://lea.verou.me
 */(function(){var e=/\blang(?:uage)?-(?!\*)(\w+)\b/i,t=self.Prism={util:{type:function(e){return Object.prototype.toString.call(e).match(/\[object (\w+)\]/)[1]},clone:function(e){var n=t.util.type(e);switch(n){case"Object":var r={};for(var i in e)e.hasOwnProperty(i)&&(r[i]=t.util.clone(e[i]));return r;case"Array":return e.slice()}return e}},languages:{extend:function(e,n){var r=t.util.clone(t.languages[e]);for(var i in n)r[i]=n[i];return r},insertBefore:function(e,n,r,i){i=i||t.languages;var s=i[e],o={};for(var u in s)if(s.hasOwnProperty(u)){if(u==n)for(var a in r)r.hasOwnProperty(a)&&(o[a]=r[a]);o[u]=s[u]}return i[e]=o},DFS:function(e,n){for(var r in e){n.call(e,r,e[r]);t.util.type(e)==="Object"&&t.languages.DFS(e[r],n)}}},highlightAll:function(e,n){var r=document.querySelectorAll('code[class*="language-"], [class*="language-"] code, code[class*="lang-"], [class*="lang-"] code');for(var i=0,s;s=r[i++];)t.highlightElement(s,e===!0,n)},highlightElement:function(r,i,s){var o,u,a=r;while(a&&!e.test(a.className))a=a.parentNode;if(a){o=(a.className.match(e)||[,""])[1];u=t.languages[o]}if(!u)return;r.className=r.className.replace(e,"").replace(/\s+/g," ")+" language-"+o;a=r.parentNode;/pre/i.test(a.nodeName)&&(a.className=a.className.replace(e,"").replace(/\s+/g," ")+" language-"+o);var f=r.textContent;if(!f)return;f=f.replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/\u00a0/g," ");var l={element:r,language:o,grammar:u,code:f};t.hooks.run("before-highlight",l);if(i&&self.Worker){var c=new Worker(t.filename);c.onmessage=function(e){l.highlightedCode=n.stringify(JSON.parse(e.data),o);t.hooks.run("before-insert",l);l.element.innerHTML=l.highlightedCode;s&&s.call(l.element);t.hooks.run("after-highlight",l)};c.postMessage(JSON.stringify({language:l.language,code:l.code}))}else{l.highlightedCode=t.highlight(l.code,l.grammar,l.language);t.hooks.run("before-insert",l);l.element.innerHTML=l.highlightedCode;s&&s.call(r);t.hooks.run("after-highlight",l)}},highlight:function(e,r,i){return n.stringify(t.tokenize(e,r),i)},tokenize:function(e,n,r){var i=t.Token,s=[e],o=n.rest;if(o){for(var u in o)n[u]=o[u];delete n.rest}e:for(var u in n){if(!n.hasOwnProperty(u)||!n[u])continue;var a=n[u],f=a.inside,l=!!a.lookbehind,c=0;a=a.pattern||a;for(var h=0;h<s.length;h++){var p=s[h];if(s.length>e.length)break e;if(p instanceof i)continue;a.lastIndex=0;var d=a.exec(p);if(d){l&&(c=d[1].length);var v=d.index-1+c,d=d[0].slice(c),m=d.length,g=v+m,y=p.slice(0,v+1),b=p.slice(g+1),w=[h,1];y&&w.push(y);var E=new i(u,f?t.tokenize(d,f):d);w.push(E);b&&w.push(b);Array.prototype.splice.apply(s,w)}}}return s},hooks:{all:{},add:function(e,n){var r=t.hooks.all;r[e]=r[e]||[];r[e].push(n)},run:function(e,n){var r=t.hooks.all[e];if(!r||!r.length)return;for(var i=0,s;s=r[i++];)s(n)}}},n=t.Token=function(e,t){this.type=e;this.content=t};n.stringify=function(e,r,i){if(typeof e=="string")return e;if(Object.prototype.toString.call(e)=="[object Array]")return e.map(function(t){return n.stringify(t,r,e)}).join("");var s={type:e.type,content:n.stringify(e.content,r,i),tag:"span",classes:["token",e.type],attributes:{},language:r,parent:i};s.type=="comment"&&(s.attributes.spellcheck="true");t.hooks.run("wrap",s);var o="";for(var u in s.attributes)o+=u+'="'+(s.attributes[u]||"")+'"';return"<"+s.tag+' class="'+s.classes.join(" ")+'" '+o+">"+s.content+"</"+s.tag+">"};if(!self.document){self.addEventListener("message",function(e){var n=JSON.parse(e.data),r=n.language,i=n.code;self.postMessage(JSON.stringify(t.tokenize(i,t.languages[r])));self.close()},!1);return}var r=document.getElementsByTagName("script");r=r[r.length-1];if(r){t.filename=r.src;document.addEventListener&&!r.hasAttribute("data-manual")&&document.addEventListener("DOMContentLoaded",t.highlightAll)}})();;
Prism.languages.markup={comment:/&lt;!--[\w\W]*?-->/g,prolog:/&lt;\?.+?\?>/,doctype:/&lt;!DOCTYPE.+?>/,cdata:/&lt;!\[CDATA\[[\w\W]*?]]>/i,tag:{pattern:/&lt;\/?[\w:-]+\s*(?:\s+[\w:-]+(?:=(?:("|')(\\?[\w\W])*?\1|\w+))?\s*)*\/?>/gi,inside:{tag:{pattern:/^&lt;\/?[\w:-]+/i,inside:{punctuation:/^&lt;\/?/,namespace:/^[\w-]+?:/}},"attr-value":{pattern:/=(?:('|")[\w\W]*?(\1)|[^\s>]+)/gi,inside:{punctuation:/=|>|"/g}},punctuation:/\/?>/g,"attr-name":{pattern:/[\w:-]+/g,inside:{namespace:/^[\w-]+?:/}}}},entity:/&amp;#?[\da-z]{1,8};/gi};Prism.hooks.add("wrap",function(e){e.type==="entity"&&(e.attributes.title=e.content.replace(/&amp;/,"&"))});;
Prism.languages.css={comment:/\/\*[\w\W]*?\*\//g,atrule:{pattern:/@[\w-]+?.*?(;|(?=\s*{))/gi,inside:{punctuation:/[;:]/g}},url:/url\((["']?).*?\1\)/gi,selector:/[^\{\}\s][^\{\};]*(?=\s*\{)/g,property:/(\b|\B)[\w-]+(?=\s*:)/ig,string:/("|')(\\?.)*?\1/g,important:/\B!important\b/gi,ignore:/&(lt|gt|amp);/gi,punctuation:/[\{\};:]/g};Prism.languages.markup&&Prism.languages.insertBefore("markup","tag",{style:{pattern:/(&lt;|<)style[\w\W]*?(>|&gt;)[\w\W]*?(&lt;|<)\/style(>|&gt;)/ig,inside:{tag:{pattern:/(&lt;|<)style[\w\W]*?(>|&gt;)|(&lt;|<)\/style(>|&gt;)/ig,inside:Prism.languages.markup.tag.inside},rest:Prism.languages.css}}});;
Prism.languages.clike={comment:{pattern:/(^|[^\\])(\/\*[\w\W]*?\*\/|(^|[^:])\/\/.*?(\r?\n|$))/g,lookbehind:!0},string:/("|')(\\?.)*?\1/g,"class-name":{pattern:/((?:(?:class|interface|extends|implements|trait|instanceof|new)\s+)|(?:catch\s+\())[a-z0-9_\.\\]+/ig,lookbehind:!0,inside:{punctuation:/(\.|\\)/}},keyword:/\b(if|else|while|do|for|return|in|instanceof|function|new|try|throw|catch|finally|null|break|continue)\b/g,"boolean":/\b(true|false)\b/g,"function":{pattern:/[a-z0-9_]+\(/ig,inside:{punctuation:/\(/}}, number:/\b-?(0x[\dA-Fa-f]+|\d*\.?\d+([Ee]-?\d+)?)([fdi]?)\b/g,operator:/[-+]{1,2}|!|&lt;=?|>=?|={1,3}|(&amp;){1,2}|\|?\||\?|\*|\/|\~|\^|\%/g,ignore:/&(lt|gt|amp);/gi,punctuation:/[{}[\];(),.:]/g};
;
Prism.languages.javascript=Prism.languages.extend("clike",{keyword:/\b(var|let|if|else|while|do|for|return|in|instanceof|function|new|with|typeof|try|throw|catch|finally|null|break|continue)\b/g,number:/\b-?(0x[\dA-Fa-f]+|\d*\.?\d+([Ee]-?\d+)?|NaN|-?Infinity)([fdi]?)\b/g});Prism.languages.insertBefore("javascript","keyword",{regex:{pattern:/(^|[^/])\/(?!\/)(\[.+?]|\\.|[^/\r\n])+\/[gim]{0,3}(?=\s*($|[\r\n,.;})]))/g,lookbehind:!0}});Prism.languages.markup&&Prism.languages.insertBefore("markup","tag",{script:{pattern:/(&lt;|<)script[\w\W]*?(>|&gt;)[\w\W]*?(&lt;|<)\/script(>|&gt;)/ig,inside:{tag:{pattern:/(&lt;|<)script[\w\W]*?(>|&gt;)|(&lt;|<)\/script(>|&gt;)/ig,inside:Prism.languages.markup.tag.inside},rest:Prism.languages.javascript}}});
;
Prism.languages.c=Prism.languages.extend("clike",{keyword:/\b(asm|typeof|inline|auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\b/g,operator:/[-+]{1,2}|!=?|&lt;{1,2}=?|&gt;{1,2}=?|\-&gt;|={1,2}|\^|~|%|(&amp;){1,2}|\|?\||\?|\*|\//g});Prism.languages.insertBefore("c","keyword",{property:/#\s*[a-zA-Z]+/g});
;
Prism.languages.cpp=Prism.languages.extend("c",{keyword:/\b(alignas|alignof|asm|auto|bool|break|case|catch|char|char16_t|char32_t|class|compl|const|constexpr|const_cast|continue|decltype|default|delete|delete\[\]|do|double|dynamic_cast|else|enum|explicit|export|extern|float|for|friend|goto|if|inline|int|long|mutable|namespace|new|new\[\]|noexcept|nullptr|operator|private|protected|public|register|reinterpret_cast|return|short|signed|sizeof|static|static_assert|static_cast|struct|switch|template|this|thread_local|throw|try|typedef|typeid|typename|union|unsigned|using|virtual|void|volatile|wchar_t|while)\b/g,
operator:/[-+]{1,2}|!=?|&lt;{1,2}=?|&gt;{1,2}=?|\-&gt;|:{1,2}|={1,2}|\^|~|%|(&amp;){1,2}|\|?\||\?|\*|\/|\b(and|and_eq|bitand|bitor|not|not_eq|or|or_eq|xor|xor_eq)\b/g});
;

/*
 * zClip :: jQuery ZeroClipboard v1.1.1
 * http://steamdev.com/zclip
 *
 * Copyright 2011, SteamDev
 * Released under the MIT license.
 * http://www.opensource.org/licenses/mit-license.php
 *
 * Date: Wed Jun 01, 2011
 */

(function(a){a.fn.zclip=function(c){if(typeof c=="object"&&!c.length){var b=a.extend({path:"ZeroClipboard.swf",copy:null,beforeCopy:null,afterCopy:null,clickAfter:true,setHandCursor:true,setCSSEffects:true},c);return this.each(function(){var e=a(this);if(e.is(":visible")&&(typeof b.copy=="string"||a.isFunction(b.copy))){ZeroClipboard.setMoviePath(b.path);var d=new ZeroClipboard.Client();if(a.isFunction(b.copy)){e.bind("zClip_copy",b.copy)}if(a.isFunction(b.beforeCopy)){e.bind("zClip_beforeCopy",b.beforeCopy)}if(a.isFunction(b.afterCopy)){e.bind("zClip_afterCopy",b.afterCopy)}d.setHandCursor(b.setHandCursor);d.setCSSEffects(b.setCSSEffects);d.addEventListener("mouseOver",function(f){e.trigger("mouseenter")});d.addEventListener("mouseOut",function(f){e.trigger("mouseleave")});d.addEventListener("mouseDown",function(f){e.trigger("mousedown");if(!a.isFunction(b.copy)){d.setText(b.copy)}else{d.setText(e.triggerHandler("zClip_copy"))}if(a.isFunction(b.beforeCopy)){e.trigger("zClip_beforeCopy")}});d.addEventListener("complete",function(f,g){if(a.isFunction(b.afterCopy)){e.trigger("zClip_afterCopy")}else{if(g.length>500){g=g.substr(0,500)+"...\n\n("+(g.length-500)+" characters not shown)"}e.removeClass("hover");alert("Copied text to clipboard:\n\n "+g)}if(b.clickAfter){e.trigger("click")}});d.glue(e[0],e.parent()[0]);a(window).bind("load resize",function(){d.reposition()})}})}else{if(typeof c=="string"){return this.each(function(){var f=a(this);c=c.toLowerCase();var e=f.data("zclipId");var d=a("#"+e+".zclip");if(c=="remove"){d.remove();f.removeClass("active hover")}else{if(c=="hide"){d.hide();f.removeClass("active hover")}else{if(c=="show"){d.show()}}}})}}}})(jQuery);var ZeroClipboard={version:"1.0.7",clients:{},moviePath:"ZeroClipboard.swf",nextId:1,$:function(a){if(typeof(a)=="string"){a=document.getElementById(a)}if(!a.addClass){a.hide=function(){this.style.display="none"};a.show=function(){this.style.display=""};a.addClass=function(b){this.removeClass(b);this.className+=" "+b};a.removeClass=function(d){var e=this.className.split(/\s+/);var b=-1;for(var c=0;c<e.length;c++){if(e[c]==d){b=c;c=e.length}}if(b>-1){e.splice(b,1);this.className=e.join(" ")}return this};a.hasClass=function(b){return !!this.className.match(new RegExp("\\s*"+b+"\\s*"))}}return a},setMoviePath:function(a){this.moviePath=a},dispatch:function(d,b,c){var a=this.clients[d];if(a){a.receiveEvent(b,c)}},register:function(b,a){this.clients[b]=a},getDOMObjectPosition:function(c,a){var b={left:0,top:0,width:c.width?c.width:c.offsetWidth,height:c.height?c.height:c.offsetHeight};if(c&&(c!=a)){b.left+=c.offsetLeft;b.top+=c.offsetTop}return b},Client:function(a){this.handlers={};this.id=ZeroClipboard.nextId++;this.movieId="ZeroClipboardMovie_"+this.id;ZeroClipboard.register(this.id,this);if(a){this.glue(a)}}};ZeroClipboard.Client.prototype={id:0,ready:false,movie:null,clipText:"",handCursorEnabled:true,cssEffects:true,handlers:null,glue:function(d,b,e){this.domElement=ZeroClipboard.$(d);var f=99;if(this.domElement.style.zIndex){f=parseInt(this.domElement.style.zIndex,10)+1}if(typeof(b)=="string"){b=ZeroClipboard.$(b)}else{if(typeof(b)=="undefined"){b=document.getElementsByTagName("body")[0]}}var c=ZeroClipboard.getDOMObjectPosition(this.domElement,b);this.div=document.createElement("div");this.div.className="zclip";this.div.id="zclip-"+this.movieId;$(this.domElement).data("zclipId","zclip-"+this.movieId);var a=this.div.style;a.position="absolute";a.left=""+c.left+"px";a.top=""+c.top+"px";a.width=""+c.width+"px";a.height=""+c.height+"px";a.zIndex=f;if(typeof(e)=="object"){for(addedStyle in e){a[addedStyle]=e[addedStyle]}}b.appendChild(this.div);this.div.innerHTML=this.getHTML(c.width,c.height)},getHTML:function(d,a){var c="";var b="id="+this.id+"&width="+d+"&height="+a;if(navigator.userAgent.match(/MSIE/)){var e=location.href.match(/^https/i)?"https://":"http://";c+='<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="'+e+'download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,0,0" width="'+d+'" height="'+a+'" id="'+this.movieId+'" align="middle"><param name="allowScriptAccess" value="always" /><param name="allowFullScreen" value="false" /><param name="movie" value="'+ZeroClipboard.moviePath+'" /><param name="loop" value="false" /><param name="menu" value="false" /><param name="quality" value="best" /><param name="bgcolor" value="#ffffff" /><param name="flashvars" value="'+b+'"/><param name="wmode" value="transparent"/></object>'}else{c+='<embed id="'+this.movieId+'" src="'+ZeroClipboard.moviePath+'" loop="false" menu="false" quality="best" bgcolor="#ffffff" width="'+d+'" height="'+a+'" name="'+this.movieId+'" align="middle" allowScriptAccess="always" allowFullScreen="false" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" flashvars="'+b+'" wmode="transparent" />'}return c},hide:function(){if(this.div){this.div.style.left="-2000px"}},show:function(){this.reposition()},destroy:function(){if(this.domElement&&this.div){this.hide();this.div.innerHTML="";var a=document.getElementsByTagName("body")[0];try{a.removeChild(this.div)}catch(b){}this.domElement=null;this.div=null}},reposition:function(c){if(c){this.domElement=ZeroClipboard.$(c);if(!this.domElement){this.hide()}}if(this.domElement&&this.div){var b=ZeroClipboard.getDOMObjectPosition(this.domElement);var a=this.div.style;a.left=""+b.left+"px";a.top=""+b.top+"px"}},setText:function(a){this.clipText=a;if(this.ready){this.movie.setText(a)}},addEventListener:function(a,b){a=a.toString().toLowerCase().replace(/^on/,"");if(!this.handlers[a]){this.handlers[a]=[]}this.handlers[a].push(b)},setHandCursor:function(a){this.handCursorEnabled=a;if(this.ready){this.movie.setHandCursor(a)}},setCSSEffects:function(a){this.cssEffects=!!a},receiveEvent:function(d,f){d=d.toString().toLowerCase().replace(/^on/,"");switch(d){case"load":this.movie=document.getElementById(this.movieId);if(!this.movie){var c=this;setTimeout(function(){c.receiveEvent("load",null)},1);return}if(!this.ready&&navigator.userAgent.match(/Firefox/)&&navigator.userAgent.match(/Windows/)){var c=this;setTimeout(function(){c.receiveEvent("load",null)},100);this.ready=true;return}this.ready=true;try{this.movie.setText(this.clipText)}catch(h){}try{this.movie.setHandCursor(this.handCursorEnabled)}catch(h){}break;case"mouseover":if(this.domElement&&this.cssEffects){this.domElement.addClass("hover");if(this.recoverActive){this.domElement.addClass("active")}}break;case"mouseout":if(this.domElement&&this.cssEffects){this.recoverActive=false;if(this.domElement.hasClass("active")){this.domElement.removeClass("active");this.recoverActive=true}this.domElement.removeClass("hover")}break;case"mousedown":if(this.domElement&&this.cssEffects){this.domElement.addClass("active")}break;case"mouseup":if(this.domElement&&this.cssEffects){this.domElement.removeClass("active");this.recoverActive=false}break}if(this.handlers[d]){for(var b=0,a=this.handlers[d].length;b<a;b++){var g=this.handlers[d][b];if(typeof(g)=="function"){g(this,f)}else{if((typeof(g)=="object")&&(g.length==2)){g[0][g[1]](this,f)}else{if(typeof(g)=="string"){window[g](this,f)}}}}}}};
/**
 * dat-gui JavaScript Controller Library
 * http://code.google.com/p/dat-gui
 *
 * Copyright 2011 Data Arts Team, Google Creative Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 */

/** @namespace */
var dat = dat || {};

/** @namespace */
dat.gui = dat.gui || {};

/** @namespace */
dat.utils = dat.utils || {};

/** @namespace */
dat.controllers = dat.controllers || {};

/** @namespace */
dat.dom = dat.dom || {};

/** @namespace */
dat.color = dat.color || {};

dat.utils.css = (function () {
  return {
    load: function (url, doc) {
      doc = doc || document;
      var link = doc.createElement('link');
      link.type = 'text/css';
      link.rel = 'stylesheet';
      link.href = url;
      doc.getElementsByTagName('head')[0].appendChild(link);
    },
    inject: function(css, doc) {
      doc = doc || document;
      var injected = document.createElement('style');
      injected.type = 'text/css';
      injected.innerHTML = css;
      doc.getElementsByTagName('head')[0].appendChild(injected);
    }
  }
})();


dat.utils.common = (function () {
  
  var ARR_EACH = Array.prototype.forEach;
  var ARR_SLICE = Array.prototype.slice;

  /**
   * Band-aid methods for things that should be a lot easier in JavaScript.
   * Implementation and structure inspired by underscore.js
   * http://documentcloud.github.com/underscore/
   */

  return { 
    
    BREAK: {},
  
    extend: function(target) {
      
      this.each(ARR_SLICE.call(arguments, 1), function(obj) {
        
        for (var key in obj)
          if (!this.isUndefined(obj[key])) 
            target[key] = obj[key];
        
      }, this);
      
      return target;
      
    },
    
    defaults: function(target) {
      
      this.each(ARR_SLICE.call(arguments, 1), function(obj) {
        
        for (var key in obj)
          if (this.isUndefined(target[key])) 
            target[key] = obj[key];
        
      }, this);
      
      return target;
    
    },
    
    compose: function() {
      var toCall = ARR_SLICE.call(arguments);
            return function() {
              var args = ARR_SLICE.call(arguments);
              for (var i = toCall.length -1; i >= 0; i--) {
                args = [toCall[i].apply(this, args)];
              }
              return args[0];
            }
    },
    
    each: function(obj, itr, scope) {

      
      if (ARR_EACH && obj.forEach === ARR_EACH) { 
        
        obj.forEach(itr, scope);
        
      } else if (obj.length === obj.length + 0) { // Is number but not NaN
        
        for (var key = 0, l = obj.length; key < l; key++)
          if (key in obj && itr.call(scope, obj[key], key) === this.BREAK) 
            return;
            
      } else {

        for (var key in obj) 
          if (itr.call(scope, obj[key], key) === this.BREAK)
            return;
            
      }
            
    },
    
    defer: function(fnc) {
      setTimeout(fnc, 0);
    },
    
    toArray: function(obj) {
      if (obj.toArray) return obj.toArray();
      return ARR_SLICE.call(obj);
    },

    isUndefined: function(obj) {
      return obj === undefined;
    },
    
    isNull: function(obj) {
      return obj === null;
    },
    
    isNaN: function(obj) {
      return obj !== obj;
    },
    
    isArray: Array.isArray || function(obj) {
      return obj.constructor === Array;
    },
    
    isObject: function(obj) {
      return obj === Object(obj);
    },
    
    isNumber: function(obj) {
      return obj === obj+0;
    },
    
    isString: function(obj) {
      return obj === obj+'';
    },
    
    isBoolean: function(obj) {
      return obj === false || obj === true;
    },
    
    isFunction: function(obj) {
      return Object.prototype.toString.call(obj) === '[object Function]';
    }
  
  };
    
})();


dat.controllers.Controller = (function (common) {

  /**
   * @class An "abstract" class that represents a given property of an object.
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   *
   * @member dat.controllers
   */
  var Controller = function(object, property) {

    this.initialValue = object[property];

    /**
     * Those who extend this class will put their DOM elements in here.
     * @type {DOMElement}
     */
    this.domElement = document.createElement('div');

    /**
     * The object to manipulate
     * @type {Object}
     */
    this.object = object;

    /**
     * The name of the property to manipulate
     * @type {String}
     */
    this.property = property;

    /**
     * The function to be called on change.
     * @type {Function}
     * @ignore
     */
    this.__onChange = undefined;

    /**
     * The function to be called on finishing change.
     * @type {Function}
     * @ignore
     */
    this.__onFinishChange = undefined;

  };

  common.extend(

      Controller.prototype,

      /** @lends dat.controllers.Controller.prototype */
      {

        /**
         * Specify that a function fire every time someone changes the value with
         * this Controller.
         *
         * @param {Function} fnc This function will be called whenever the value
         * is modified via this Controller.
         * @returns {dat.controllers.Controller} this
         */
        onChange: function(fnc) {
          this.__onChange = fnc;
          return this;
        },

        /**
         * Specify that a function fire every time someone "finishes" changing
         * the value wih this Controller. Useful for values that change
         * incrementally like numbers or strings.
         *
         * @param {Function} fnc This function will be called whenever
         * someone "finishes" changing the value via this Controller.
         * @returns {dat.controllers.Controller} this
         */
        onFinishChange: function(fnc) {
          this.__onFinishChange = fnc;
          return this;
        },

        /**
         * Change the value of <code>object[property]</code>
         *
         * @param {Object} newValue The new value of <code>object[property]</code>
         */
        setValue: function(newValue) {
          this.object[this.property] = newValue;
          if (this.__onChange) {
            this.__onChange.call(this, newValue);
          }
          this.updateDisplay();
          return this;
        },

        /**
         * Gets the value of <code>object[property]</code>
         *
         * @returns {Object} The current value of <code>object[property]</code>
         */
        getValue: function() {
          return this.object[this.property];
        },

        /**
         * Refreshes the visual display of a Controller in order to keep sync
         * with the object's current value.
         * @returns {dat.controllers.Controller} this
         */
        updateDisplay: function() {
          return this;
        },

        /**
         * @returns {Boolean} true if the value has deviated from initialValue
         */
        isModified: function() {
          return this.initialValue !== this.getValue()
        }

      }

  );

  return Controller;


})(dat.utils.common);


dat.dom.dom = (function (common) {

  var EVENT_MAP = {
    'HTMLEvents': ['change'],
    'MouseEvents': ['click','mousemove','mousedown','mouseup', 'mouseover'],
    'KeyboardEvents': ['keydown']
  };

  var EVENT_MAP_INV = {};
  common.each(EVENT_MAP, function(v, k) {
    common.each(v, function(e) {
      EVENT_MAP_INV[e] = k;
    });
  });

  var CSS_VALUE_PIXELS = /(\d+(\.\d+)?)px/;

  function cssValueToPixels(val) {

    if (val === '0' || common.isUndefined(val)) return 0;

    var match = val.match(CSS_VALUE_PIXELS);

    if (!common.isNull(match)) {
      return parseFloat(match[1]);
    }

    // TODO ...ems? %?

    return 0;

  }

  /**
   * @namespace
   * @member dat.dom
   */
  var dom = {

    /**
     * 
     * @param elem
     * @param selectable
     */
    makeSelectable: function(elem, selectable) {

      if (elem === undefined || elem.style === undefined) return;

      elem.onselectstart = selectable ? function() {
        return false;
      } : function() {
      };

      elem.style.MozUserSelect = selectable ? 'auto' : 'none';
      elem.style.KhtmlUserSelect = selectable ? 'auto' : 'none';
      elem.unselectable = selectable ? 'on' : 'off';

    },

    /**
     *
     * @param elem
     * @param horizontal
     * @param vertical
     */
    makeFullscreen: function(elem, horizontal, vertical) {

      if (common.isUndefined(horizontal)) horizontal = true;
      if (common.isUndefined(vertical)) vertical = true;

      elem.style.position = 'absolute';

      if (horizontal) {
        elem.style.left = 0;
        elem.style.right = 0;
      }
      if (vertical) {
        elem.style.top = 0;
        elem.style.bottom = 0;
      }

    },

    /**
     *
     * @param elem
     * @param eventType
     * @param params
     */
    fakeEvent: function(elem, eventType, params, aux) {
      params = params || {};
      var className = EVENT_MAP_INV[eventType];
      if (!className) {
        throw new Error('Event type ' + eventType + ' not supported.');
      }
      var evt = document.createEvent(className);
      switch (className) {
        case 'MouseEvents':
          var clientX = params.x || params.clientX || 0;
          var clientY = params.y || params.clientY || 0;
          evt.initMouseEvent(eventType, params.bubbles || false,
              params.cancelable || true, window, params.clickCount || 1,
              0, //screen X
              0, //screen Y
              clientX, //client X
              clientY, //client Y
              false, false, false, false, 0, null);
          break;
        case 'KeyboardEvents':
          var init = evt.initKeyboardEvent || evt.initKeyEvent; // webkit || moz
          common.defaults(params, {
            cancelable: true,
            ctrlKey: false,
            altKey: false,
            shiftKey: false,
            metaKey: false,
            keyCode: undefined,
            charCode: undefined
          });
          init(eventType, params.bubbles || false,
              params.cancelable, window,
              params.ctrlKey, params.altKey,
              params.shiftKey, params.metaKey,
              params.keyCode, params.charCode);
          break;
        default:
          evt.initEvent(eventType, params.bubbles || false,
              params.cancelable || true);
          break;
      }
      common.defaults(evt, aux);
      elem.dispatchEvent(evt);
    },

    /**
     *
     * @param elem
     * @param event
     * @param func
     * @param bool
     */
    bind: function(elem, event, func, bool) {
      bool = bool || false;
      if (elem.addEventListener)
        elem.addEventListener(event, func, bool);
      else if (elem.attachEvent)
        elem.attachEvent('on' + event, func);
      return dom;
    },

    /**
     *
     * @param elem
     * @param event
     * @param func
     * @param bool
     */
    unbind: function(elem, event, func, bool) {
      bool = bool || false;
      if (elem.removeEventListener)
        elem.removeEventListener(event, func, bool);
      else if (elem.detachEvent)
        elem.detachEvent('on' + event, func);
      return dom;
    },

    /**
     *
     * @param elem
     * @param className
     */
    addClass: function(elem, className) {
      if (elem.className === undefined) {
        elem.className = className;
      } else if (elem.className !== className) {
        var classes = elem.className.split(/ +/);
        if (classes.indexOf(className) == -1) {
          classes.push(className);
          elem.className = classes.join(' ').replace(/^\s+/, '').replace(/\s+$/, '');
        }
      }
      return dom;
    },

    /**
     *
     * @param elem
     * @param className
     */
    removeClass: function(elem, className) {
      if (className) {
        if (elem.className === undefined) {
          // elem.className = className;
        } else if (elem.className === className) {
          elem.removeAttribute('class');
        } else {
          var classes = elem.className.split(/ +/);
          var index = classes.indexOf(className);
          if (index != -1) {
            classes.splice(index, 1);
            elem.className = classes.join(' ');
          }
        }
      } else {
        elem.className = undefined;
      }
      return dom;
    },

    hasClass: function(elem, className) {
      return new RegExp('(?:^|\\s+)' + className + '(?:\\s+|$)').test(elem.className) || false;
    },

    /**
     *
     * @param elem
     */
    getWidth: function(elem) {

      var style = getComputedStyle(elem);

      return cssValueToPixels(style['border-left-width']) +
          cssValueToPixels(style['border-right-width']) +
          cssValueToPixels(style['padding-left']) +
          cssValueToPixels(style['padding-right']) +
          cssValueToPixels(style['width']);
    },

    /**
     *
     * @param elem
     */
    getHeight: function(elem) {

      var style = getComputedStyle(elem);

      return cssValueToPixels(style['border-top-width']) +
          cssValueToPixels(style['border-bottom-width']) +
          cssValueToPixels(style['padding-top']) +
          cssValueToPixels(style['padding-bottom']) +
          cssValueToPixels(style['height']);
    },

    /**
     *
     * @param elem
     */
    getOffset: function(elem) {
      var offset = {left: 0, top:0};
      if (elem.offsetParent) {
        do {
          offset.left += elem.offsetLeft;
          offset.top += elem.offsetTop;
        } while (elem = elem.offsetParent);
      }
      return offset;
    },

    // http://stackoverflow.com/posts/2684561/revisions
    /**
     * 
     * @param elem
     */
    isActive: function(elem) {
      return elem === document.activeElement && ( elem.type || elem.href );
    }

  };

  return dom;

})(dat.utils.common);


dat.controllers.OptionController = (function (Controller, dom, common) {

  /**
   * @class Provides a select input to alter the property of an object, using a
   * list of accepted values.
   *
   * @extends dat.controllers.Controller
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   * @param {Object|string[]} options A map of labels to acceptable values, or
   * a list of acceptable string values.
   *
   * @member dat.controllers
   */
  var OptionController = function(object, property, options) {

    OptionController.superclass.call(this, object, property);

    var _this = this;

    /**
     * The drop down menu
     * @ignore
     */
    this.__select = document.createElement('select');

    if (common.isArray(options)) {
      var map = {};
      common.each(options, function(element) {
        map[element] = element;
      });
      options = map;
    }

    common.each(options, function(value, key) {

      var opt = document.createElement('option');
      opt.innerHTML = key;
      opt.setAttribute('value', value);
      _this.__select.appendChild(opt);

    });

    // Acknowledge original value
    this.updateDisplay();

    dom.bind(this.__select, 'change', function() {
      var desiredValue = this.options[this.selectedIndex].value;
      _this.setValue(desiredValue);
    });

    this.domElement.appendChild(this.__select);

  };

  OptionController.superclass = Controller;

  common.extend(

      OptionController.prototype,
      Controller.prototype,

      {

        setValue: function(v) {
          var toReturn = OptionController.superclass.prototype.setValue.call(this, v);
          if (this.__onFinishChange) {
            this.__onFinishChange.call(this, this.getValue());
          }
          return toReturn;
        },

        updateDisplay: function() {
          this.__select.value = this.getValue();
          return OptionController.superclass.prototype.updateDisplay.call(this);
        }

      }

  );

  return OptionController;

})(dat.controllers.Controller,
dat.dom.dom,
dat.utils.common);


dat.controllers.NumberController = (function (Controller, common) {

  /**
   * @class Represents a given property of an object that is a number.
   *
   * @extends dat.controllers.Controller
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   * @param {Object} [params] Optional parameters
   * @param {Number} [params.min] Minimum allowed value
   * @param {Number} [params.max] Maximum allowed value
   * @param {Number} [params.step] Increment by which to change value
   *
   * @member dat.controllers
   */
  var NumberController = function(object, property, params) {

    NumberController.superclass.call(this, object, property);

    params = params || {};

    this.__min = params.min;
    this.__max = params.max;
    this.__step = params.step;

    if (common.isUndefined(this.__step)) {

      if (this.initialValue == 0) {
        this.__impliedStep = 1; // What are we, psychics?
      } else {
        // Hey Doug, check this out.
        this.__impliedStep = Math.pow(10, Math.floor(Math.log(this.initialValue)/Math.LN10))/10;
      }

    } else {

    	this.__impliedStep = this.__step;

    }

    this.__precision = numDecimals(this.__impliedStep);


  };

  NumberController.superclass = Controller;

  common.extend(

      NumberController.prototype,
      Controller.prototype,

      /** @lends dat.controllers.NumberController.prototype */
      {

        setValue: function(v) {

          if (this.__min !== undefined && v < this.__min) {
            v = this.__min;
          } else if (this.__max !== undefined && v > this.__max) {
            v = this.__max;
          }

          if (this.__step !== undefined && v % this.__step != 0) {
            v = Math.round(v / this.__step) * this.__step;
          }

          return NumberController.superclass.prototype.setValue.call(this, v);

        },

        /**
         * Specify a minimum value for <code>object[property]</code>.
         *
         * @param {Number} minValue The minimum value for
         * <code>object[property]</code>
         * @returns {dat.controllers.NumberController} this
         */
        min: function(v) {
          this.__min = v;
          return this;
        },

        /**
         * Specify a maximum value for <code>object[property]</code>.
         *
         * @param {Number} maxValue The maximum value for
         * <code>object[property]</code>
         * @returns {dat.controllers.NumberController} this
         */
        max: function(v) {
          this.__max = v;
          return this;
        },

        /**
         * Specify a step value that dat.controllers.NumberController
         * increments by.
         *
         * @param {Number} stepValue The step value for
         * dat.controllers.NumberController
         * @default if minimum and maximum specified increment is 1% of the
         * difference otherwise stepValue is 1
         * @returns {dat.controllers.NumberController} this
         */
        step: function(v) {
          this.__step = v;
          return this;
        }

      }

  );

  function numDecimals(x) {
    x = x.toString();
    if (x.indexOf('.') > -1) {
      return x.length - x.indexOf('.') - 1;
    } else {
      return 0;
    }
  }

  return NumberController;

})(dat.controllers.Controller,
dat.utils.common);


dat.controllers.NumberControllerBox = (function (NumberController, dom, common) {

  /**
   * @class Represents a given property of an object that is a number and
   * provides an input element with which to manipulate it.
   *
   * @extends dat.controllers.Controller
   * @extends dat.controllers.NumberController
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   * @param {Object} [params] Optional parameters
   * @param {Number} [params.min] Minimum allowed value
   * @param {Number} [params.max] Maximum allowed value
   * @param {Number} [params.step] Increment by which to change value
   *
   * @member dat.controllers
   */
  var NumberControllerBox = function(object, property, params) {

    this.__truncationSuspended = false;

    NumberControllerBox.superclass.call(this, object, property, params);

    var _this = this;

    /**
     * {Number} Previous mouse y position
     * @ignore
     */
    var prev_y;

    this.__input = document.createElement('input');
    this.__input.setAttribute('type', 'text');

    // Makes it so manually specified values are not truncated.

    dom.bind(this.__input, 'change', onChange);
    dom.bind(this.__input, 'blur', onBlur);
    dom.bind(this.__input, 'mousedown', onMouseDown);
    dom.bind(this.__input, 'keydown', function(e) {

      // When pressing entire, you can be as precise as you want.
      if (e.keyCode === 13) {
        _this.__truncationSuspended = true;
        this.blur();
        _this.__truncationSuspended = false;
      }

    });

    function onChange() {
      var attempted = parseFloat(_this.__input.value);
      if (!common.isNaN(attempted)) _this.setValue(attempted);
    }

    function onBlur() {
      onChange();
      if (_this.__onFinishChange) {
        _this.__onFinishChange.call(_this, _this.getValue());
      }
    }

    function onMouseDown(e) {
      dom.bind(window, 'mousemove', onMouseDrag);
      dom.bind(window, 'mouseup', onMouseUp);
      prev_y = e.clientY;
    }

    function onMouseDrag(e) {

      var diff = prev_y - e.clientY;
      _this.setValue(_this.getValue() + diff * _this.__impliedStep);

      prev_y = e.clientY;

    }

    function onMouseUp() {
      dom.unbind(window, 'mousemove', onMouseDrag);
      dom.unbind(window, 'mouseup', onMouseUp);
    }

    this.updateDisplay();

    this.domElement.appendChild(this.__input);

  };

  NumberControllerBox.superclass = NumberController;

  common.extend(

      NumberControllerBox.prototype,
      NumberController.prototype,

      {

        updateDisplay: function() {

          this.__input.value = this.__truncationSuspended ? this.getValue() : roundToDecimal(this.getValue(), this.__precision);
          return NumberControllerBox.superclass.prototype.updateDisplay.call(this);
        }

      }

  );

  function roundToDecimal(value, decimals) {
    var tenTo = Math.pow(10, decimals);
    return Math.round(value * tenTo) / tenTo;
  }

  return NumberControllerBox;

})(dat.controllers.NumberController,
dat.dom.dom,
dat.utils.common);


dat.controllers.NumberControllerSlider = (function (NumberController, dom, css, common, styleSheet) {

  /**
   * @class Represents a given property of an object that is a number, contains
   * a minimum and maximum, and provides a slider element with which to
   * manipulate it. It should be noted that the slider element is made up of
   * <code>&lt;div&gt;</code> tags, <strong>not</strong> the html5
   * <code>&lt;slider&gt;</code> element.
   *
   * @extends dat.controllers.Controller
   * @extends dat.controllers.NumberController
   * 
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   * @param {Number} minValue Minimum allowed value
   * @param {Number} maxValue Maximum allowed value
   * @param {Number} stepValue Increment by which to change value
   *
   * @member dat.controllers
   */
  var NumberControllerSlider = function(object, property, min, max, step) {

    NumberControllerSlider.superclass.call(this, object, property, { min: min, max: max, step: step });

    var _this = this;

    this.__background = document.createElement('div');
    this.__foreground = document.createElement('div');
    


    dom.bind(this.__background, 'mousedown', onMouseDown);
    
    dom.addClass(this.__background, 'slider');
    dom.addClass(this.__foreground, 'slider-fg');

    function onMouseDown(e) {

      dom.bind(window, 'mousemove', onMouseDrag);
      dom.bind(window, 'mouseup', onMouseUp);

      onMouseDrag(e);
    }

    function onMouseDrag(e) {

      e.preventDefault();

      var offset = dom.getOffset(_this.__background);
      var width = dom.getWidth(_this.__background);
      
      _this.setValue(
      	map(e.clientX, offset.left, offset.left + width, _this.__min, _this.__max)
      );

      return false;

    }

    function onMouseUp() {
      dom.unbind(window, 'mousemove', onMouseDrag);
      dom.unbind(window, 'mouseup', onMouseUp);
      if (_this.__onFinishChange) {
        _this.__onFinishChange.call(_this, _this.getValue());
      }
    }

    this.updateDisplay();

    this.__background.appendChild(this.__foreground);
    this.domElement.appendChild(this.__background);

  };

  NumberControllerSlider.superclass = NumberController;

  /**
   * Injects default stylesheet for slider elements.
   */
  NumberControllerSlider.useDefaultStyles = function() {
    css.inject(styleSheet);
  };

  common.extend(

      NumberControllerSlider.prototype,
      NumberController.prototype,

      {

        updateDisplay: function() {
          var pct = (this.getValue() - this.__min)/(this.__max - this.__min);
          this.__foreground.style.width = pct*100+'%';
          return NumberControllerSlider.superclass.prototype.updateDisplay.call(this);
        }

      }



  );

	function map(v, i1, i2, o1, o2) {
		return o1 + (o2 - o1) * ((v - i1) / (i2 - i1));
	}

  return NumberControllerSlider;
  
})(dat.controllers.NumberController,
dat.dom.dom,
dat.utils.css,
dat.utils.common,
"/**\n * dat-gui JavaScript Controller Library\n * http://code.google.com/p/dat-gui\n *\n * Copyright 2011 Data Arts Team, Google Creative Lab\n *\n * Licensed under the Apache License, Version 2.0 (the \"License\");\n * you may not use this file except in compliance with the License.\n * You may obtain a copy of the License at\n *\n * http://www.apache.org/licenses/LICENSE-2.0\n */\n\n.slider {\n  box-shadow: inset 0 2px 4px rgba(0,0,0,0.15);\n  height: 1em;\n  border-radius: 1em;\n  background-color: #eee;\n  padding: 0 0.5em;\n  overflow: hidden;\n}\n\n.slider-fg {\n  padding: 1px 0 2px 0;\n  background-color: #aaa;\n  height: 1em;\n  margin-left: -0.5em;\n  padding-right: 0.5em;\n  border-radius: 1em 0 0 1em;\n}\n\n.slider-fg:after {\n  display: inline-block;\n  border-radius: 1em;\n  background-color: #fff;\n  border:  1px solid #aaa;\n  content: '';\n  float: right;\n  margin-right: -1em;\n  margin-top: -1px;\n  height: 0.9em;\n  width: 0.9em;\n}");


dat.controllers.FunctionController = (function (Controller, dom, common) {

  /**
   * @class Provides a GUI interface to fire a specified method, a property of an object.
   *
   * @extends dat.controllers.Controller
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   *
   * @member dat.controllers
   */
  var FunctionController = function(object, property, text) {

    FunctionController.superclass.call(this, object, property);

    var _this = this;

    this.__button = document.createElement('div');
    this.__button.innerHTML = text === undefined ? 'Fire' : text;
    dom.bind(this.__button, 'click', function(e) {
      e.preventDefault();
      _this.fire();
      return false;
    });

    dom.addClass(this.__button, 'button');

    this.domElement.appendChild(this.__button);


  };

  FunctionController.superclass = Controller;

  common.extend(

      FunctionController.prototype,
      Controller.prototype,
      {
        
        fire: function() {
          if (this.__onChange) {
            this.__onChange.call(this);
          }
          if (this.__onFinishChange) {
            this.__onFinishChange.call(this, this.getValue());
          }
          this.getValue().call(this.object);
        }
      }

  );

  return FunctionController;

})(dat.controllers.Controller,
dat.dom.dom,
dat.utils.common);


dat.controllers.BooleanController = (function (Controller, dom, common) {

  /**
   * @class Provides a checkbox input to alter the boolean property of an object.
   * @extends dat.controllers.Controller
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   *
   * @member dat.controllers
   */
  var BooleanController = function(object, property) {

    BooleanController.superclass.call(this, object, property);

    var _this = this;
    this.__prev = this.getValue();

    this.__checkbox = document.createElement('input');
    this.__checkbox.setAttribute('type', 'checkbox');


    dom.bind(this.__checkbox, 'change', onChange, false);

    this.domElement.appendChild(this.__checkbox);

    // Match original value
    this.updateDisplay();

    function onChange() {
      _this.setValue(!_this.__prev);
    }

  };

  BooleanController.superclass = Controller;

  common.extend(

      BooleanController.prototype,
      Controller.prototype,

      {

        setValue: function(v) {
          var toReturn = BooleanController.superclass.prototype.setValue.call(this, v);
          if (this.__onFinishChange) {
            this.__onFinishChange.call(this, this.getValue());
          }
          this.__prev = this.getValue();
          return toReturn;
        },

        updateDisplay: function() {
          
          if (this.getValue() === true) {
            this.__checkbox.setAttribute('checked', 'checked');
            this.__checkbox.checked = true;    
          } else {
              this.__checkbox.checked = false;
          }

          return BooleanController.superclass.prototype.updateDisplay.call(this);

        }


      }

  );

  return BooleanController;

})(dat.controllers.Controller,
dat.dom.dom,
dat.utils.common);


dat.color.toString = (function (common) {

  return function(color) {

    if (color.a == 1 || common.isUndefined(color.a)) {

      var s = color.hex.toString(16);
      while (s.length < 6) {
        s = '0' + s;
      }

      return '#' + s;

    } else {

      return 'rgba(' + Math.round(color.r) + ',' + Math.round(color.g) + ',' + Math.round(color.b) + ',' + color.a + ')';

    }

  }

})(dat.utils.common);


dat.color.interpret = (function (toString, common) {

  var result, toReturn;

  var interpret = function() {

    toReturn = false;

    var original = arguments.length > 1 ? common.toArray(arguments) : arguments[0];

    common.each(INTERPRETATIONS, function(family) {

      if (family.litmus(original)) {

        common.each(family.conversions, function(conversion, conversionName) {

          result = conversion.read(original);

          if (toReturn === false && result !== false) {
            toReturn = result;
            result.conversionName = conversionName;
            result.conversion = conversion;
            return common.BREAK;

          }

        });

        return common.BREAK;

      }

    });

    return toReturn;

  };

  var INTERPRETATIONS = [

    // Strings
    {

      litmus: common.isString,

      conversions: {

        THREE_CHAR_HEX: {

          read: function(original) {

            var test = original.match(/^#([A-F0-9])([A-F0-9])([A-F0-9])$/i);
            if (test === null) return false;

            return {
              space: 'HEX',
              hex: parseInt(
                  '0x' +
                      test[1].toString() + test[1].toString() +
                      test[2].toString() + test[2].toString() +
                      test[3].toString() + test[3].toString())
            };

          },

          write: toString

        },

        SIX_CHAR_HEX: {

          read: function(original) {

            var test = original.match(/^#([A-F0-9]{6})$/i);
            if (test === null) return false;

            return {
              space: 'HEX',
              hex: parseInt('0x' + test[1].toString())
            };

          },

          write: toString

        },

        CSS_RGB: {

          read: function(original) {

            var test = original.match(/^rgb\(\s*(.+)\s*,\s*(.+)\s*,\s*(.+)\s*\)/);
            if (test === null) return false;

            return {
              space: 'RGB',
              r: parseFloat(test[1]),
              g: parseFloat(test[2]),
              b: parseFloat(test[3])
            };

          },

          write: toString

        },

        CSS_RGBA: {

          read: function(original) {

            var test = original.match(/^rgba\(\s*(.+)\s*,\s*(.+)\s*,\s*(.+)\s*\,\s*(.+)\s*\)/);
            if (test === null) return false;

            return {
              space: 'RGB',
              r: parseFloat(test[1]),
              g: parseFloat(test[2]),
              b: parseFloat(test[3]),
              a: parseFloat(test[4])
            };

          },

          write: toString

        }

      }

    },

    // Numbers
    {

      litmus: common.isNumber,

      conversions: {

        HEX: {
          read: function(original) {
            return {
              space: 'HEX',
              hex: original,
              conversionName: 'HEX'
            }
          },

          write: function(color) {
            return color.hex;
          }
        }

      }

    },

    // Arrays
    {

      litmus: common.isArray,

      conversions: {

        RGB_ARRAY: {
          read: function(original) {
            if (original.length != 3) return false;
            return {
              space: 'RGB',
              r: original[0],
              g: original[1],
              b: original[2]
            };
          },

          write: function(color) {
            return [color.r, color.g, color.b];
          }

        },

        RGBA_ARRAY: {
          read: function(original) {
            if (original.length != 4) return false;
            return {
              space: 'RGB',
              r: original[0],
              g: original[1],
              b: original[2],
              a: original[3]
            };
          },

          write: function(color) {
            return [color.r, color.g, color.b, color.a];
          }

        }

      }

    },

    // Objects
    {

      litmus: common.isObject,

      conversions: {

        RGBA_OBJ: {
          read: function(original) {
            if (common.isNumber(original.r) &&
                common.isNumber(original.g) &&
                common.isNumber(original.b) &&
                common.isNumber(original.a)) {
              return {
                space: 'RGB',
                r: original.r,
                g: original.g,
                b: original.b,
                a: original.a
              }
            }
            return false;
          },

          write: function(color) {
            return {
              r: color.r,
              g: color.g,
              b: color.b,
              a: color.a
            }
          }
        },

        RGB_OBJ: {
          read: function(original) {
            if (common.isNumber(original.r) &&
                common.isNumber(original.g) &&
                common.isNumber(original.b)) {
              return {
                space: 'RGB',
                r: original.r,
                g: original.g,
                b: original.b
              }
            }
            return false;
          },

          write: function(color) {
            return {
              r: color.r,
              g: color.g,
              b: color.b
            }
          }
        },

        HSVA_OBJ: {
          read: function(original) {
            if (common.isNumber(original.h) &&
                common.isNumber(original.s) &&
                common.isNumber(original.v) &&
                common.isNumber(original.a)) {
              return {
                space: 'HSV',
                h: original.h,
                s: original.s,
                v: original.v,
                a: original.a
              }
            }
            return false;
          },

          write: function(color) {
            return {
              h: color.h,
              s: color.s,
              v: color.v,
              a: color.a
            }
          }
        },

        HSV_OBJ: {
          read: function(original) {
            if (common.isNumber(original.h) &&
                common.isNumber(original.s) &&
                common.isNumber(original.v)) {
              return {
                space: 'HSV',
                h: original.h,
                s: original.s,
                v: original.v
              }
            }
            return false;
          },

          write: function(color) {
            return {
              h: color.h,
              s: color.s,
              v: color.v
            }
          }

        }

      }

    }


  ];

  return interpret;


})(dat.color.toString,
dat.utils.common);


dat.GUI = dat.gui.GUI = (function (css, saveDialogueContents, styleSheet, controllerFactory, Controller, BooleanController, FunctionController, NumberControllerBox, NumberControllerSlider, OptionController, ColorController, requestAnimationFrame, CenteredDiv, dom, common) {

  css.inject(styleSheet);

  /** Outer-most className for GUI's */
  var CSS_NAMESPACE = 'dg';

  var HIDE_KEY_CODE = 72;

  /** The only value shared between the JS and SCSS. Use caution. */
  var CLOSE_BUTTON_HEIGHT = 20;

  var DEFAULT_DEFAULT_PRESET_NAME = 'Default';

  var SUPPORTS_LOCAL_STORAGE = (function() {
    try {
      return 'localStorage' in window && window['localStorage'] !== null;
    } catch (e) {
      return false;
    }
  })();

  var SAVE_DIALOGUE;

  /** Have we yet to create an autoPlace GUI? */
  var auto_place_virgin = true;

  /** Fixed position div that auto place GUI's go inside */
  var auto_place_container;

  /** Are we hiding the GUI's ? */
  var hide = false;

  /** GUI's which should be hidden */
  var hideable_guis = [];

  /**
   * A lightweight controller library for JavaScript. It allows you to easily
   * manipulate variables and fire functions on the fly.
   * @class
   *
   * @member dat.gui
   *
   * @param {Object} [params]
   * @param {String} [params.name] The name of this GUI.
   * @param {Object} [params.load] JSON object representing the saved state of
   * this GUI.
   * @param {Boolean} [params.auto=true]
   * @param {dat.gui.GUI} [params.parent] The GUI I'm nested in.
   * @param {Boolean} [params.closed] If true, starts closed
   */
  var GUI = function(params) {

    var _this = this;

    /**
     * Outermost DOM Element
     * @type DOMElement
     */
    this.domElement = document.createElement('div');
    this.__ul = document.createElement('ul');
    this.domElement.appendChild(this.__ul);

    dom.addClass(this.domElement, CSS_NAMESPACE);

    /**
     * Nested GUI's by name
     * @ignore
     */
    this.__folders = {};

    this.__controllers = [];

    /**
     * List of objects I'm remembering for save, only used in top level GUI
     * @ignore
     */
    this.__rememberedObjects = [];

    /**
     * Maps the index of remembered objects to a map of controllers, only used
     * in top level GUI.
     *
     * @private
     * @ignore
     *
     * @example
     * [
     *  {
     *    propertyName: Controller,
     *    anotherPropertyName: Controller
     *  },
     *  {
     *    propertyName: Controller
     *  }
     * ]
     */
    this.__rememberedObjectIndecesToControllers = [];

    this.__listening = [];

    params = params || {};

    // Default parameters
    params = common.defaults(params, {
      autoPlace: true,
      width: GUI.DEFAULT_WIDTH
    });

    params = common.defaults(params, {
      resizable: params.autoPlace,
      hideable: params.autoPlace
    });


    if (!common.isUndefined(params.load)) {

      // Explicit preset
      if (params.preset) params.load.preset = params.preset;

    } else {

      params.load = { preset: DEFAULT_DEFAULT_PRESET_NAME };

    }

    if (common.isUndefined(params.parent) && params.hideable) {
      hideable_guis.push(this);
    }

    // Only root level GUI's are resizable.
    params.resizable = common.isUndefined(params.parent) && params.resizable;


    if (params.autoPlace && common.isUndefined(params.scrollable)) {
      params.scrollable = true;
    }
//    params.scrollable = common.isUndefined(params.parent) && params.scrollable === true;

    // Not part of params because I don't want people passing this in via
    // constructor. Should be a 'remembered' value.
    var use_local_storage =
        SUPPORTS_LOCAL_STORAGE &&
            localStorage.getItem(getLocalStorageHash(this, 'isLocal')) === 'true';

    Object.defineProperties(this,

        /** @lends dat.gui.GUI.prototype */
        {

          /**
           * The parent <code>GUI</code>
           * @type dat.gui.GUI
           */
          parent: {
            get: function() {
              return params.parent;
            }
          },

          scrollable: {
            get: function() {
              return params.scrollable;
            }
          },

          /**
           * Handles <code>GUI</code>'s element placement for you
           * @type Boolean
           */
          autoPlace: {
            get: function() {
              return params.autoPlace;
            }
          },

          /**
           * The identifier for a set of saved values
           * @type String
           */
          preset: {

            get: function() {
              if (_this.parent) {
                return _this.getRoot().preset;
              } else {
                return params.load.preset;
              }
            },

            set: function(v) {
              if (_this.parent) {
                _this.getRoot().preset = v;
              } else {
                params.load.preset = v;
              }
              setPresetSelectIndex(this);
              _this.revert();
            }

          },

          /**
           * The width of <code>GUI</code> element
           * @type Number
           */
          width: {
            get: function() {
              return params.width;
            },
            set: function(v) {
              params.width = v;
              setWidth(_this, v);
            }
          },

          /**
           * The name of <code>GUI</code>. Used for folders. i.e
           * a folder's name
           * @type String
           */
          name: {
            get: function() {
              return params.name;
            },
            set: function(v) {
              // TODO Check for collisions among sibling folders
              params.name = v;
              if (title_row_name) {
                title_row_name.innerHTML = params.name;
              }
            }
          },

          /**
           * Whether the <code>GUI</code> is collapsed or not
           * @type Boolean
           */
          closed: {
            get: function() {
              return params.closed;
            },
            set: function(v) {
              params.closed = v;
              if (params.closed) {
                dom.addClass(_this.__ul, GUI.CLASS_CLOSED);
              } else {
                dom.removeClass(_this.__ul, GUI.CLASS_CLOSED);
              }
              // For browsers that aren't going to respect the CSS transition,
              // Lets just check our height against the window height right off
              // the bat.
              this.onResize();

              if (_this.__closeButton) {
                _this.__closeButton.innerHTML = v ? GUI.TEXT_OPEN : GUI.TEXT_CLOSED;
              }
            }
          },

          /**
           * Contains all presets
           * @type Object
           */
          load: {
            get: function() {
              return params.load;
            }
          },

          /**
           * Determines whether or not to use <a href="https://developer.mozilla.org/en/DOM/Storage#localStorage">localStorage</a> as the means for
           * <code>remember</code>ing
           * @type Boolean
           */
          useLocalStorage: {

            get: function() {
              return use_local_storage;
            },
            set: function(bool) {
              if (SUPPORTS_LOCAL_STORAGE) {
                use_local_storage = bool;
                if (bool) {
                  dom.bind(window, 'unload', saveToLocalStorage);
                } else {
                  dom.unbind(window, 'unload', saveToLocalStorage);
                }
                localStorage.setItem(getLocalStorageHash(_this, 'isLocal'), bool);
              }
            }

          }

        });

    // Are we a root level GUI?
    if (common.isUndefined(params.parent)) {

      params.closed = false;

      dom.addClass(this.domElement, GUI.CLASS_MAIN);
      dom.makeSelectable(this.domElement, false);

      // Are we supposed to be loading locally?
      if (SUPPORTS_LOCAL_STORAGE) {

        if (use_local_storage) {

          _this.useLocalStorage = true;

          var saved_gui = localStorage.getItem(getLocalStorageHash(this, 'gui'));

          if (saved_gui) {
            params.load = JSON.parse(saved_gui);
          }

        }

      }

      this.__closeButton = document.createElement('div');
      this.__closeButton.innerHTML = GUI.TEXT_CLOSED;
      dom.addClass(this.__closeButton, GUI.CLASS_CLOSE_BUTTON);
      this.domElement.appendChild(this.__closeButton);

      dom.bind(this.__closeButton, 'click', function() {

        _this.closed = !_this.closed;


      });


      // Oh, you're a nested GUI!
    } else {

      if (params.closed === undefined) {
        params.closed = true;
      }

      var title_row_name = document.createTextNode(params.name);
      dom.addClass(title_row_name, 'controller-name');

      var title_row = addRow(_this, title_row_name);

      var on_click_title = function(e) {
        e.preventDefault();
        _this.closed = !_this.closed;
        return false;
      };

      dom.addClass(this.__ul, GUI.CLASS_CLOSED);

      dom.addClass(title_row, 'title');
      dom.bind(title_row, 'click', on_click_title);

      if (!params.closed) {
        this.closed = false;
      }

    }

    if (params.autoPlace) {

      if (common.isUndefined(params.parent)) {

        if (auto_place_virgin) {
          auto_place_container = document.createElement('div');
          dom.addClass(auto_place_container, CSS_NAMESPACE);
          dom.addClass(auto_place_container, GUI.CLASS_AUTO_PLACE_CONTAINER);
          document.body.appendChild(auto_place_container);
          auto_place_virgin = false;
        }

        // Put it in the dom for you.
        auto_place_container.appendChild(this.domElement);

        // Apply the auto styles
        dom.addClass(this.domElement, GUI.CLASS_AUTO_PLACE);

      }


      // Make it not elastic.
      if (!this.parent) setWidth(_this, params.width);

    }

    dom.bind(window, 'resize', function() { _this.onResize() });
    dom.bind(this.__ul, 'webkitTransitionEnd', function() { _this.onResize(); });
    dom.bind(this.__ul, 'transitionend', function() { _this.onResize() });
    dom.bind(this.__ul, 'oTransitionEnd', function() { _this.onResize() });
    this.onResize();


    if (params.resizable) {
      addResizeHandle(this);
    }

    function saveToLocalStorage() {
      localStorage.setItem(getLocalStorageHash(_this, 'gui'), JSON.stringify(_this.getSaveObject()));
    }

    var root = _this.getRoot();
    function resetWidth() {
	      var root = _this.getRoot();
	      root.width += 1;
	      common.defer(function() {
	        root.width -= 1;
	      });
	    }

	    if (!params.parent) {
	      resetWidth();
	    }

  };

  GUI.toggleHide = function() {

    hide = !hide;
    common.each(hideable_guis, function(gui) {
      gui.domElement.style.zIndex = hide ? -999 : 999;
      gui.domElement.style.opacity = hide ? 0 : 1;
    });
  };

  GUI.CLASS_AUTO_PLACE = 'a';
  GUI.CLASS_AUTO_PLACE_CONTAINER = 'ac';
  GUI.CLASS_MAIN = 'main';
  GUI.CLASS_CONTROLLER_ROW = 'cr';
  GUI.CLASS_TOO_TALL = 'taller-than-window';
  GUI.CLASS_CLOSED = 'closed';
  GUI.CLASS_CLOSE_BUTTON = 'close-button';
  GUI.CLASS_DRAG = 'drag';

  GUI.DEFAULT_WIDTH = 245;
  GUI.TEXT_CLOSED = 'Close Controls';
  GUI.TEXT_OPEN = 'Open Controls';

  dom.bind(window, 'keydown', function(e) {

    if (document.activeElement.type !== 'text' &&
        (e.which === HIDE_KEY_CODE || e.keyCode == HIDE_KEY_CODE)) {
      GUI.toggleHide();
    }

  }, false);

  common.extend(

      GUI.prototype,

      /** @lends dat.gui.GUI */
      {

        /**
         * @param object
         * @param property
         * @returns {dat.controllers.Controller} The new controller that was added.
         * @instance
         */
        add: function(object, property) {

          return add(
              this,
              object,
              property,
              {
                factoryArgs: Array.prototype.slice.call(arguments, 2)
              }
          );

        },

        /**
         * @param object
         * @param property
         * @returns {dat.controllers.ColorController} The new controller that was added.
         * @instance
         */
        addColor: function(object, property) {

          return add(
              this,
              object,
              property,
              {
                color: true
              }
          );

        },

        /**
         * @param controller
         * @instance
         */
        remove: function(controller) {

          // TODO listening?
          this.__ul.removeChild(controller.__li);
          this.__controllers.slice(this.__controllers.indexOf(controller), 1);
          var _this = this;
          common.defer(function() {
            _this.onResize();
          });

        },

        destroy: function() {

          if (this.autoPlace) {
            auto_place_container.removeChild(this.domElement);
          }

        },

        /**
         * @param name
         * @returns {dat.gui.GUI} The new folder.
         * @throws {Error} if this GUI already has a folder by the specified
         * name
         * @instance
         */
        addFolder: function(name) {

          // We have to prevent collisions on names in order to have a key
          // by which to remember saved values
          if (this.__folders[name] !== undefined) {
            throw new Error('You already have a folder in this GUI by the' +
                ' name "' + name + '"');
          }

          var new_gui_params = { name: name, parent: this };

          // We need to pass down the autoPlace trait so that we can
          // attach event listeners to open/close folder actions to
          // ensure that a scrollbar appears if the window is too short.
          new_gui_params.autoPlace = this.autoPlace;

          // Do we have saved appearance data for this folder?

          if (this.load && // Anything loaded?
              this.load.folders && // Was my parent a dead-end?
              this.load.folders[name]) { // Did daddy remember me?

            // Start me closed if I was closed
            new_gui_params.closed = this.load.folders[name].closed;

            // Pass down the loaded data
            new_gui_params.load = this.load.folders[name];

          }

          var gui = new GUI(new_gui_params);
          this.__folders[name] = gui;

          var li = addRow(this, gui.domElement);
          dom.addClass(li, 'folder');
          return gui;

        },

        open: function() {
          this.closed = false;
        },

        close: function() {
          this.closed = true;
        },

        onResize: function() {

          var root = this.getRoot();

          if (root.scrollable) {

            var top = dom.getOffset(root.__ul).top;
            var h = 0;

            common.each(root.__ul.childNodes, function(node) {
              if (! (root.autoPlace && node === root.__save_row))
                h += dom.getHeight(node);
            });

            if (window.innerHeight - top - CLOSE_BUTTON_HEIGHT < h) {
              dom.addClass(root.domElement, GUI.CLASS_TOO_TALL);
              root.__ul.style.height = window.innerHeight - top - CLOSE_BUTTON_HEIGHT + 'px';
            } else {
              dom.removeClass(root.domElement, GUI.CLASS_TOO_TALL);
              root.__ul.style.height = 'auto';
            }

          }

          if (root.__resize_handle) {
            common.defer(function() {
              root.__resize_handle.style.height = root.__ul.offsetHeight + 'px';
            });
          }

          if (root.__closeButton) {
            root.__closeButton.style.width = root.width + 'px';
          }

        },

        /**
         * Mark objects for saving. The order of these objects cannot change as
         * the GUI grows. When remembering new objects, append them to the end
         * of the list.
         *
         * @param {Object...} objects
         * @throws {Error} if not called on a top level GUI.
         * @instance
         */
        remember: function() {

          if (common.isUndefined(SAVE_DIALOGUE)) {
            SAVE_DIALOGUE = new CenteredDiv();
            SAVE_DIALOGUE.domElement.innerHTML = saveDialogueContents;
          }

          if (this.parent) {
            throw new Error("You can only call remember on a top level GUI.");
          }

          var _this = this;

          common.each(Array.prototype.slice.call(arguments), function(object) {
            if (_this.__rememberedObjects.length == 0) {
              addSaveMenu(_this);
            }
            if (_this.__rememberedObjects.indexOf(object) == -1) {
              _this.__rememberedObjects.push(object);
            }
          });

          if (this.autoPlace) {
            // Set save row width
            setWidth(this, this.width);
          }

        },

        /**
         * @returns {dat.gui.GUI} the topmost parent GUI of a nested GUI.
         * @instance
         */
        getRoot: function() {
          var gui = this;
          while (gui.parent) {
            gui = gui.parent;
          }
          return gui;
        },

        /**
         * @returns {Object} a JSON object representing the current state of
         * this GUI as well as its remembered properties.
         * @instance
         */
        getSaveObject: function() {

          var toReturn = this.load;

          toReturn.closed = this.closed;

          // Am I remembering any values?
          if (this.__rememberedObjects.length > 0) {

            toReturn.preset = this.preset;

            if (!toReturn.remembered) {
              toReturn.remembered = {};
            }

            toReturn.remembered[this.preset] = getCurrentPreset(this);

          }

          toReturn.folders = {};
          common.each(this.__folders, function(element, key) {
            toReturn.folders[key] = element.getSaveObject();
          });

          return toReturn;

        },

        save: function() {

          if (!this.load.remembered) {
            this.load.remembered = {};
          }

          this.load.remembered[this.preset] = getCurrentPreset(this);
          markPresetModified(this, false);

        },

        saveAs: function(presetName) {

          if (!this.load.remembered) {

            // Retain default values upon first save
            this.load.remembered = {};
            this.load.remembered[DEFAULT_DEFAULT_PRESET_NAME] = getCurrentPreset(this, true);

          }

          this.load.remembered[presetName] = getCurrentPreset(this);
          this.preset = presetName;
          addPresetOption(this, presetName, true);

        },

        revert: function(gui) {

          common.each(this.__controllers, function(controller) {
            // Make revert work on Default.
            if (!this.getRoot().load.remembered) {
              controller.setValue(controller.initialValue);
            } else {
              recallSavedValue(gui || this.getRoot(), controller);
            }
          }, this);

          common.each(this.__folders, function(folder) {
            folder.revert(folder);
          });

          if (!gui) {
            markPresetModified(this.getRoot(), false);
          }


        },

        listen: function(controller) {

          var init = this.__listening.length == 0;
          this.__listening.push(controller);
          if (init) updateDisplays(this.__listening);

        }

      }

  );

  function add(gui, object, property, params) {

    if (object[property] === undefined) {
      throw new Error("Object " + object + " has no property \"" + property + "\"");
    }

    var controller;

    if (params.color) {

      controller = new ColorController(object, property);

    } else {

      var factoryArgs = [object,property].concat(params.factoryArgs);
      controller = controllerFactory.apply(gui, factoryArgs);

    }

    if (params.before instanceof Controller) {
      params.before = params.before.__li;
    }

    recallSavedValue(gui, controller);

    dom.addClass(controller.domElement, 'c');

    var name = document.createElement('span');
    dom.addClass(name, 'property-name');
    name.innerHTML = controller.property;

    var container = document.createElement('div');
    container.appendChild(name);
    container.appendChild(controller.domElement);

    var li = addRow(gui, container, params.before);

    dom.addClass(li, GUI.CLASS_CONTROLLER_ROW);
    dom.addClass(li, typeof controller.getValue());

    augmentController(gui, li, controller);

    gui.__controllers.push(controller);

    return controller;

  }

  /**
   * Add a row to the end of the GUI or before another row.
   *
   * @param gui
   * @param [dom] If specified, inserts the dom content in the new row
   * @param [liBefore] If specified, places the new row before another row
   */
  function addRow(gui, dom, liBefore) {
    var li = document.createElement('li');
    if (dom) li.appendChild(dom);
    if (liBefore) {
      gui.__ul.insertBefore(li, params.before);
    } else {
      gui.__ul.appendChild(li);
    }
    gui.onResize();
    return li;
  }

  function augmentController(gui, li, controller) {

    controller.__li = li;
    controller.__gui = gui;

    common.extend(controller, {

      options: function(options) {

        if (arguments.length > 1) {
          controller.remove();

          return add(
              gui,
              controller.object,
              controller.property,
              {
                before: controller.__li.nextElementSibling,
                factoryArgs: [common.toArray(arguments)]
              }
          );

        }

        if (common.isArray(options) || common.isObject(options)) {
          controller.remove();

          return add(
              gui,
              controller.object,
              controller.property,
              {
                before: controller.__li.nextElementSibling,
                factoryArgs: [options]
              }
          );

        }

      },

      name: function(v) {
        controller.__li.firstElementChild.firstElementChild.innerHTML = v;
        return controller;
      },

      listen: function() {
        controller.__gui.listen(controller);
        return controller;
      },

      remove: function() {
        controller.__gui.remove(controller);
        return controller;
      }

    });

    // All sliders should be accompanied by a box.
    if (controller instanceof NumberControllerSlider) {

      var box = new NumberControllerBox(controller.object, controller.property,
          { min: controller.__min, max: controller.__max, step: controller.__step });

      common.each(['updateDisplay', 'onChange', 'onFinishChange'], function(method) {
        var pc = controller[method];
        var pb = box[method];
        controller[method] = box[method] = function() {
          var args = Array.prototype.slice.call(arguments);
          pc.apply(controller, args);
          return pb.apply(box, args);
        }
      });

      dom.addClass(li, 'has-slider');
      controller.domElement.insertBefore(box.domElement, controller.domElement.firstElementChild);

    }
    else if (controller instanceof NumberControllerBox) {

      var r = function(returned) {

        // Have we defined both boundaries?
        if (common.isNumber(controller.__min) && common.isNumber(controller.__max)) {

          // Well, then lets just replace this with a slider.
          controller.remove();
          return add(
              gui,
              controller.object,
              controller.property,
              {
                before: controller.__li.nextElementSibling,
                factoryArgs: [controller.__min, controller.__max, controller.__step]
              });

        }

        return returned;

      };

      controller.min = common.compose(r, controller.min);
      controller.max = common.compose(r, controller.max);

    }
    else if (controller instanceof BooleanController) {

      dom.bind(li, 'click', function() {
        dom.fakeEvent(controller.__checkbox, 'click');
      });

      dom.bind(controller.__checkbox, 'click', function(e) {
        e.stopPropagation(); // Prevents double-toggle
      })

    }
    else if (controller instanceof FunctionController) {

      dom.bind(li, 'click', function() {
        dom.fakeEvent(controller.__button, 'click');
      });

      dom.bind(li, 'mouseover', function() {
        dom.addClass(controller.__button, 'hover');
      });

      dom.bind(li, 'mouseout', function() {
        dom.removeClass(controller.__button, 'hover');
      });

    }
    else if (controller instanceof ColorController) {

      dom.addClass(li, 'color');
      controller.updateDisplay = common.compose(function(r) {
        li.style.borderLeftColor = controller.__color.toString();
        return r;
      }, controller.updateDisplay);

      controller.updateDisplay();

    }

    controller.setValue = common.compose(function(r) {
      if (gui.getRoot().__preset_select && controller.isModified()) {
        markPresetModified(gui.getRoot(), true);
      }
      return r;
    }, controller.setValue);

  }

  function recallSavedValue(gui, controller) {

    // Find the topmost GUI, that's where remembered objects live.
    var root = gui.getRoot();

    // Does the object we're controlling match anything we've been told to
    // remember?
    var matched_index = root.__rememberedObjects.indexOf(controller.object);

    // Why yes, it does!
    if (matched_index != -1) {

      // Let me fetch a map of controllers for thcommon.isObject.
      var controller_map =
          root.__rememberedObjectIndecesToControllers[matched_index];

      // Ohp, I believe this is the first controller we've created for this
      // object. Lets make the map fresh.
      if (controller_map === undefined) {
        controller_map = {};
        root.__rememberedObjectIndecesToControllers[matched_index] =
            controller_map;
      }

      // Keep track of this controller
      controller_map[controller.property] = controller;

      // Okay, now have we saved any values for this controller?
      if (root.load && root.load.remembered) {

        var preset_map = root.load.remembered;

        // Which preset are we trying to load?
        var preset;

        if (preset_map[gui.preset]) {

          preset = preset_map[gui.preset];

        } else if (preset_map[DEFAULT_DEFAULT_PRESET_NAME]) {

          // Uhh, you can have the default instead?
          preset = preset_map[DEFAULT_DEFAULT_PRESET_NAME];

        } else {

          // Nada.

          return;

        }


        // Did the loaded object remember thcommon.isObject?
        if (preset[matched_index] &&

          // Did we remember this particular property?
            preset[matched_index][controller.property] !== undefined) {

          // We did remember something for this guy ...
          var value = preset[matched_index][controller.property];

          // And that's what it is.
          controller.initialValue = value;
          controller.setValue(value);

        }

      }

    }

  }

  function getLocalStorageHash(gui, key) {
    // TODO how does this deal with multiple GUI's?
    return document.location.href + '.' + key;

  }

  function addSaveMenu(gui) {

    var div = gui.__save_row = document.createElement('li');

    dom.addClass(gui.domElement, 'has-save');

    gui.__ul.insertBefore(div, gui.__ul.firstChild);

    dom.addClass(div, 'save-row');

    var gears = document.createElement('span');
    gears.innerHTML = '&nbsp;';
    dom.addClass(gears, 'button gears');

    // TODO replace with FunctionController
    var button = document.createElement('span');
    button.innerHTML = 'Save';
    dom.addClass(button, 'button');
    dom.addClass(button, 'save');

    var button2 = document.createElement('span');
    button2.innerHTML = 'New';
    dom.addClass(button2, 'button');
    dom.addClass(button2, 'save-as');

    var button3 = document.createElement('span');
    button3.innerHTML = 'Revert';
    dom.addClass(button3, 'button');
    dom.addClass(button3, 'revert');

    var select = gui.__preset_select = document.createElement('select');

    if (gui.load && gui.load.remembered) {

      common.each(gui.load.remembered, function(value, key) {
        addPresetOption(gui, key, key == gui.preset);
      });

    } else {
      addPresetOption(gui, DEFAULT_DEFAULT_PRESET_NAME, false);
    }

    dom.bind(select, 'change', function() {


      for (var index = 0; index < gui.__preset_select.length; index++) {
        gui.__preset_select[index].innerHTML = gui.__preset_select[index].value;
      }

      gui.preset = this.value;

    });

    div.appendChild(select);
    div.appendChild(gears);
    div.appendChild(button);
    div.appendChild(button2);
    div.appendChild(button3);

    if (SUPPORTS_LOCAL_STORAGE) {

      var saveLocally = document.getElementById('dg-save-locally');
      var explain = document.getElementById('dg-local-explain');

      saveLocally.style.display = 'block';

      var localStorageCheckBox = document.getElementById('dg-local-storage');

      if (localStorage.getItem(getLocalStorageHash(gui, 'isLocal')) === 'true') {
        localStorageCheckBox.setAttribute('checked', 'checked');
      }

      function showHideExplain() {
        explain.style.display = gui.useLocalStorage ? 'block' : 'none';
      }

      showHideExplain();

      // TODO: Use a boolean controller, fool!
      dom.bind(localStorageCheckBox, 'change', function() {
        gui.useLocalStorage = !gui.useLocalStorage;
        showHideExplain();
      });

    }

    var newConstructorTextArea = document.getElementById('dg-new-constructor');

    dom.bind(newConstructorTextArea, 'keydown', function(e) {
      if (e.metaKey && (e.which === 67 || e.keyCode == 67)) {
        SAVE_DIALOGUE.hide();
      }
    });

    dom.bind(gears, 'click', function() {
      newConstructorTextArea.innerHTML = JSON.stringify(gui.getSaveObject(), undefined, 2);
      SAVE_DIALOGUE.show();
      newConstructorTextArea.focus();
      newConstructorTextArea.select();
    });

    dom.bind(button, 'click', function() {
      gui.save();
    });

    dom.bind(button2, 'click', function() {
      var presetName = prompt('Enter a new preset name.');
      if (presetName) gui.saveAs(presetName);
    });

    dom.bind(button3, 'click', function() {
      gui.revert();
    });

//    div.appendChild(button2);

  }

  function addResizeHandle(gui) {

    gui.__resize_handle = document.createElement('div');

    common.extend(gui.__resize_handle.style, {

      width: '6px',
      marginLeft: '-3px',
      height: '200px',
      cursor: 'ew-resize',
      position: 'absolute'
//      border: '1px solid blue'

    });

    var pmouseX;

    dom.bind(gui.__resize_handle, 'mousedown', dragStart);
    dom.bind(gui.__closeButton, 'mousedown', dragStart);

    gui.domElement.insertBefore(gui.__resize_handle, gui.domElement.firstElementChild);

    function dragStart(e) {

      e.preventDefault();

      pmouseX = e.clientX;

      dom.addClass(gui.__closeButton, GUI.CLASS_DRAG);
      dom.bind(window, 'mousemove', drag);
      dom.bind(window, 'mouseup', dragStop);

      return false;

    }

    function drag(e) {

      e.preventDefault();

      gui.width += pmouseX - e.clientX;
      gui.onResize();
      pmouseX = e.clientX;

      return false;

    }

    function dragStop() {

      dom.removeClass(gui.__closeButton, GUI.CLASS_DRAG);
      dom.unbind(window, 'mousemove', drag);
      dom.unbind(window, 'mouseup', dragStop);

    }

  }

  function setWidth(gui, w) {
    gui.domElement.style.width = w + 'px';
    // Auto placed save-rows are position fixed, so we have to
    // set the width manually if we want it to bleed to the edge
    if (gui.__save_row && gui.autoPlace) {
      gui.__save_row.style.width = w + 'px';
    }if (gui.__closeButton) {
      gui.__closeButton.style.width = w + 'px';
    }
  }

  function getCurrentPreset(gui, useInitialValues) {

    var toReturn = {};

    // For each object I'm remembering
    common.each(gui.__rememberedObjects, function(val, index) {

      var saved_values = {};

      // The controllers I've made for thcommon.isObject by property
      var controller_map =
          gui.__rememberedObjectIndecesToControllers[index];

      // Remember each value for each property
      common.each(controller_map, function(controller, property) {
        saved_values[property] = useInitialValues ? controller.initialValue : controller.getValue();
      });

      // Save the values for thcommon.isObject
      toReturn[index] = saved_values;

    });

    return toReturn;

  }

  function addPresetOption(gui, name, setSelected) {
    var opt = document.createElement('option');
    opt.innerHTML = name;
    opt.value = name;
    gui.__preset_select.appendChild(opt);
    if (setSelected) {
      gui.__preset_select.selectedIndex = gui.__preset_select.length - 1;
    }
  }

  function setPresetSelectIndex(gui) {
    for (var index = 0; index < gui.__preset_select.length; index++) {
      if (gui.__preset_select[index].value == gui.preset) {
        gui.__preset_select.selectedIndex = index;
      }
    }
  }

  function markPresetModified(gui, modified) {
    var opt = gui.__preset_select[gui.__preset_select.selectedIndex];
//    console.log('mark', modified, opt);
    if (modified) {
      opt.innerHTML = opt.value + "*";
    } else {
      opt.innerHTML = opt.value;
    }
  }

  function updateDisplays(controllerArray) {


    if (controllerArray.length != 0) {

      requestAnimationFrame(function() {
        updateDisplays(controllerArray);
      });

    }

    common.each(controllerArray, function(c) {
      c.updateDisplay();
    });

  }

  return GUI;

})(dat.utils.css,
"<div id=\"dg-save\" class=\"dg dialogue\">\n\n  Here's the new load parameter for your <code>GUI</code>'s constructor:\n\n  <textarea id=\"dg-new-constructor\"></textarea>\n\n  <div id=\"dg-save-locally\">\n\n    <input id=\"dg-local-storage\" type=\"checkbox\"/> Automatically save\n    values to <code>localStorage</code> on exit.\n\n    <div id=\"dg-local-explain\">The values saved to <code>localStorage</code> will\n      override those passed to <code>dat.GUI</code>'s constructor. This makes it\n      easier to work incrementally, but <code>localStorage</code> is fragile,\n      and your friends may not see the same values you do.\n      \n    </div>\n    \n  </div>\n\n</div>",
".dg {\n  /** Clear list styles */\n  /* Auto-place container */\n  /* Auto-placed GUI's */\n  /* Line items that don't contain folders. */\n  /** Folder names */\n  /** Hides closed items */\n  /** Controller row */\n  /** Name-half (left) */\n  /** Controller-half (right) */\n  /** Controller placement */\n  /** Shorter number boxes when slider is present. */\n  /** Ensure the entire boolean and function row shows a hand */ }\n  .dg ul {\n    list-style: none;\n    margin: 0;\n    padding: 0;\n    width: 100%;\n    clear: both; }\n  .dg.ac {\n    position: fixed;\n    top: 0;\n    left: 0;\n    right: 0;\n    height: 0;\n    z-index: 0; }\n  .dg:not(.ac) .main {\n    /** Exclude mains in ac so that we don't hide close button */\n    overflow: hidden; }\n  .dg.main {\n    -webkit-transition: opacity 0.1s linear;\n    -o-transition: opacity 0.1s linear;\n    -moz-transition: opacity 0.1s linear;\n    transition: opacity 0.1s linear; }\n    .dg.main.taller-than-window {\n      overflow-y: auto; }\n      .dg.main.taller-than-window .close-button {\n        opacity: 1;\n        /* TODO, these are style notes */\n        margin-top: -1px;\n        border-top: 1px solid #2c2c2c; }\n    .dg.main ul.closed .close-button {\n      opacity: 1 !important; }\n    .dg.main:hover .close-button,\n    .dg.main .close-button.drag {\n      opacity: 1; }\n    .dg.main .close-button {\n      /*opacity: 0;*/\n      -webkit-transition: opacity 0.1s linear;\n      -o-transition: opacity 0.1s linear;\n      -moz-transition: opacity 0.1s linear;\n      transition: opacity 0.1s linear;\n      border: 0;\n      position: absolute;\n      line-height: 19px;\n      height: 20px;\n      /* TODO, these are style notes */\n      cursor: pointer;\n      text-align: center;\n      background-color: #000; }\n      .dg.main .close-button:hover {\n        background-color: #111; }\n  .dg.a {\n    float: right;\n    margin-right: 15px;\n    overflow-x: hidden; }\n    .dg.a.has-save > ul {\n      margin-top: 27px; }\n      .dg.a.has-save > ul.closed {\n        margin-top: 0; }\n    .dg.a .save-row {\n      position: fixed;\n      top: 0;\n      z-index: 1002; }\n  .dg li {\n    -webkit-transition: height 0.1s ease-out;\n    -o-transition: height 0.1s ease-out;\n    -moz-transition: height 0.1s ease-out;\n    transition: height 0.1s ease-out; }\n  .dg li:not(.folder) {\n    cursor: auto;\n    height: 27px;\n    line-height: 27px;\n    overflow: hidden;\n    padding: 0 4px 0 5px; }\n  .dg li.folder {\n    padding: 0;\n    border-left: 4px solid rgba(0, 0, 0, 0); }\n  .dg li.title {\n    cursor: pointer;\n    margin-left: -4px; }\n  .dg .closed li:not(.title),\n  .dg .closed ul li,\n  .dg .closed ul li > * {\n    height: 0;\n    overflow: hidden;\n    border: 0; }\n  .dg .cr {\n    clear: both;\n    padding-left: 3px;\n    height: 27px; }\n  .dg .property-name {\n    cursor: default;\n    float: left;\n    clear: left;\n    width: 40%;\n    overflow: hidden;\n    text-overflow: ellipsis; }\n  .dg .c {\n    float: left;\n    width: 60%; }\n  .dg .c input[type=text] {\n    border: 0;\n    margin-top: 4px;\n    padding: 3px;\n    width: 100%;\n    float: right; }\n  .dg .has-slider input[type=text] {\n    width: 30%;\n    /*display: none;*/\n    margin-left: 0; }\n  .dg .slider {\n    float: left;\n    width: 66%;\n    margin-left: -5px;\n    margin-right: 0;\n    height: 19px;\n    margin-top: 4px; }\n  .dg .slider-fg {\n    height: 100%; }\n  .dg .c input[type=checkbox] {\n    margin-top: 9px; }\n  .dg .c select {\n    margin-top: 5px; }\n  .dg .cr.function,\n  .dg .cr.function .property-name,\n  .dg .cr.function *,\n  .dg .cr.boolean,\n  .dg .cr.boolean * {\n    cursor: pointer; }\n  .dg .selector {\n    display: none;\n    position: absolute;\n    margin-left: -9px;\n    margin-top: 23px;\n    z-index: 10; }\n  .dg .c:hover .selector,\n  .dg .selector.drag {\n    display: block; }\n  .dg li.save-row {\n    padding: 0; }\n    .dg li.save-row .button {\n      display: inline-block;\n      padding: 0px 6px; }\n  .dg.dialogue {\n    background-color: #222;\n    width: 460px;\n    padding: 15px;\n    font-size: 13px;\n    line-height: 15px; }\n\n/* TODO Separate style and structure */\n#dg-new-constructor {\n  padding: 10px;\n  color: #222;\n  font-family: Monaco, monospace;\n  font-size: 10px;\n  border: 0;\n  resize: none;\n  box-shadow: inset 1px 1px 1px #888;\n  word-wrap: break-word;\n  margin: 12px 0;\n  display: block;\n  width: 440px;\n  overflow-y: scroll;\n  height: 100px;\n  position: relative; }\n\n#dg-local-explain {\n  display: none;\n  font-size: 11px;\n  line-height: 17px;\n  border-radius: 3px;\n  background-color: #333;\n  padding: 8px;\n  margin-top: 10px; }\n  #dg-local-explain code {\n    font-size: 10px; }\n\n#dat-gui-save-locally {\n  display: none; }\n\n/** Main type */\n.dg {\n  color: #eee;\n  font: 11px 'Lucida Grande', sans-serif;\n  text-shadow: 0 -1px 0 #111;\n  /** Auto place */\n  /* Controller row, <li> */\n  /** Controllers */ }\n  .dg.main {\n    /** Scrollbar */ }\n    .dg.main::-webkit-scrollbar {\n      width: 5px;\n      background: #1a1a1a; }\n    .dg.main::-webkit-scrollbar-corner {\n      height: 0;\n      display: none; }\n    .dg.main::-webkit-scrollbar-thumb {\n      border-radius: 5px;\n      background: #676767; }\n  .dg li:not(.folder) {\n    background: #1a1a1a;\n    border-bottom: 1px solid #2c2c2c; }\n  .dg li.save-row {\n    line-height: 25px;\n    background: #dad5cb;\n    border: 0; }\n    .dg li.save-row select {\n      margin-left: 5px;\n      width: 108px; }\n    .dg li.save-row .button {\n      margin-left: 5px;\n      margin-top: 1px;\n      border-radius: 2px;\n      font-size: 9px;\n      line-height: 7px;\n      padding: 4px 4px 5px 4px;\n      background: #c5bdad;\n      color: #fff;\n      text-shadow: 0 1px 0 #b0a58f;\n      box-shadow: 0 -1px 0 #b0a58f;\n      cursor: pointer; }\n      .dg li.save-row .button.gears {\n        background: #c5bdad url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAsAAAANCAYAAAB/9ZQ7AAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAQJJREFUeNpiYKAU/P//PwGIC/ApCABiBSAW+I8AClAcgKxQ4T9hoMAEUrxx2QSGN6+egDX+/vWT4e7N82AMYoPAx/evwWoYoSYbACX2s7KxCxzcsezDh3evFoDEBYTEEqycggWAzA9AuUSQQgeYPa9fPv6/YWm/Acx5IPb7ty/fw+QZblw67vDs8R0YHyQhgObx+yAJkBqmG5dPPDh1aPOGR/eugW0G4vlIoTIfyFcA+QekhhHJhPdQxbiAIguMBTQZrPD7108M6roWYDFQiIAAv6Aow/1bFwXgis+f2LUAynwoIaNcz8XNx3Dl7MEJUDGQpx9gtQ8YCueB+D26OECAAQDadt7e46D42QAAAABJRU5ErkJggg==) 2px 1px no-repeat;\n        height: 7px;\n        width: 8px; }\n      .dg li.save-row .button:hover {\n        background-color: #bab19e;\n        box-shadow: 0 -1px 0 #b0a58f; }\n  .dg li.folder {\n    border-bottom: 0; }\n  .dg li.title {\n    padding-left: 16px;\n    background: black url(data:image/gif;base64,R0lGODlhBQAFAJEAAP////Pz8////////yH5BAEAAAIALAAAAAAFAAUAAAIIlI+hKgFxoCgAOw==) 6px 10px no-repeat;\n    cursor: pointer;\n    border-bottom: 1px solid rgba(255, 255, 255, 0.2); }\n  .dg .closed li.title {\n    background-image: url(data:image/gif;base64,R0lGODlhBQAFAJEAAP////Pz8////////yH5BAEAAAIALAAAAAAFAAUAAAIIlGIWqMCbWAEAOw==); }\n  .dg .cr.boolean {\n    border-left: 3px solid #806787; }\n  .dg .cr.function {\n    border-left: 3px solid #e61d5f; }\n  .dg .cr.number {\n    border-left: 3px solid #2fa1d6; }\n    .dg .cr.number input[type=text] {\n      color: #2fa1d6; }\n  .dg .cr.string {\n    border-left: 3px solid #1ed36f; }\n    .dg .cr.string input[type=text] {\n      color: #1ed36f; }\n  .dg .cr.function:hover, .dg .cr.boolean:hover {\n    background: #111; }\n  .dg .c input[type=text] {\n    background: #303030;\n    outline: none; }\n    .dg .c input[type=text]:hover {\n      background: #3c3c3c; }\n    .dg .c input[type=text]:focus {\n      background: #494949;\n      color: #fff; }\n  .dg .c .slider {\n    background: #303030;\n    cursor: ew-resize; }\n  .dg .c .slider-fg {\n    background: #2fa1d6; }\n  .dg .c .slider:hover {\n    background: #3c3c3c; }\n    .dg .c .slider:hover .slider-fg {\n      background: #44abda; }\n",
dat.controllers.factory = (function (OptionController, NumberControllerBox, NumberControllerSlider, StringController, FunctionController, BooleanController, common) {

      return function(object, property) {

        var initialValue = object[property];

        // Providing options?
        if (common.isArray(arguments[2]) || common.isObject(arguments[2])) {
          return new OptionController(object, property, arguments[2]);
        }

        // Providing a map?

        if (common.isNumber(initialValue)) {

          if (common.isNumber(arguments[2]) && common.isNumber(arguments[3])) {

            // Has min and max.
            return new NumberControllerSlider(object, property, arguments[2], arguments[3]);

          } else {

            return new NumberControllerBox(object, property, { min: arguments[2], max: arguments[3] });

          }

        }

        if (common.isString(initialValue)) {
          return new StringController(object, property);
        }

        if (common.isFunction(initialValue)) {
          return new FunctionController(object, property, '');
        }

        if (common.isBoolean(initialValue)) {
          return new BooleanController(object, property);
        }

      }

    })(dat.controllers.OptionController,
dat.controllers.NumberControllerBox,
dat.controllers.NumberControllerSlider,
dat.controllers.StringController = (function (Controller, dom, common) {

  /**
   * @class Provides a text input to alter the string property of an object.
   *
   * @extends dat.controllers.Controller
   *
   * @param {Object} object The object to be manipulated
   * @param {string} property The name of the property to be manipulated
   *
   * @member dat.controllers
   */
  var StringController = function(object, property) {

    StringController.superclass.call(this, object, property);

    var _this = this;

    this.__input = document.createElement('input');
    this.__input.setAttribute('type', 'text');

    dom.bind(this.__input, 'keyup', onChange);
    dom.bind(this.__input, 'change', onChange);
    dom.bind(this.__input, 'blur', onBlur);
    dom.bind(this.__input, 'keydown', function(e) {
      if (e.keyCode === 13) {
        this.blur();
      }
    });
    

    function onChange() {
      _this.setValue(_this.__input.value);
    }

    function onBlur() {
      if (_this.__onFinishChange) {
        _this.__onFinishChange.call(_this, _this.getValue());
      }
    }

    this.updateDisplay();

    this.domElement.appendChild(this.__input);

  };

  StringController.superclass = Controller;

  common.extend(

      StringController.prototype,
      Controller.prototype,

      {

        updateDisplay: function() {
          // Stops the caret from moving on account of:
          // keyup -> setValue -> updateDisplay
          if (!dom.isActive(this.__input)) {
            this.__input.value = this.getValue();
          }
          return StringController.superclass.prototype.updateDisplay.call(this);
        }

      }

  );

  return StringController;

})(dat.controllers.Controller,
dat.dom.dom,
dat.utils.common),
dat.controllers.FunctionController,
dat.controllers.BooleanController,
dat.utils.common),
dat.controllers.Controller,
dat.controllers.BooleanController,
dat.controllers.FunctionController,
dat.controllers.NumberControllerBox,
dat.controllers.NumberControllerSlider,
dat.controllers.OptionController,
dat.controllers.ColorController = (function (Controller, dom, Color, interpret, common) {

  var ColorController = function(object, property) {

    ColorController.superclass.call(this, object, property);

    this.__color = new Color(this.getValue());
    this.__temp = new Color(0);

    var _this = this;

    this.domElement = document.createElement('div');

    dom.makeSelectable(this.domElement, false);

    this.__selector = document.createElement('div');
    this.__selector.className = 'selector';

    this.__saturation_field = document.createElement('div');
    this.__saturation_field.className = 'saturation-field';

    this.__field_knob = document.createElement('div');
    this.__field_knob.className = 'field-knob';
    this.__field_knob_border = '2px solid ';

    this.__hue_knob = document.createElement('div');
    this.__hue_knob.className = 'hue-knob';

    this.__hue_field = document.createElement('div');
    this.__hue_field.className = 'hue-field';

    this.__input = document.createElement('input');
    this.__input.type = 'text';
    this.__input_textShadow = '0 1px 1px ';

    dom.bind(this.__input, 'keydown', function(e) {
      if (e.keyCode === 13) { // on enter
        onBlur.call(this);
      }
    });

    dom.bind(this.__input, 'blur', onBlur);

    dom.bind(this.__selector, 'mousedown', function(e) {

      dom
        .addClass(this, 'drag')
        .bind(window, 'mouseup', function(e) {
          dom.removeClass(_this.__selector, 'drag');
        });

    });

    var value_field = document.createElement('div');

    common.extend(this.__selector.style, {
      width: '122px',
      height: '102px',
      padding: '3px',
      backgroundColor: '#222',
      boxShadow: '0px 1px 3px rgba(0,0,0,0.3)'
    });

    common.extend(this.__field_knob.style, {
      position: 'absolute',
      width: '12px',
      height: '12px',
      border: this.__field_knob_border + (this.__color.v < .5 ? '#fff' : '#000'),
      boxShadow: '0px 1px 3px rgba(0,0,0,0.5)',
      borderRadius: '12px',
      zIndex: 1
    });
    
    common.extend(this.__hue_knob.style, {
      position: 'absolute',
      width: '15px',
      height: '2px',
      borderRight: '4px solid #fff',
      zIndex: 1
    });

    common.extend(this.__saturation_field.style, {
      width: '100px',
      height: '100px',
      border: '1px solid #555',
      marginRight: '3px',
      display: 'inline-block',
      cursor: 'pointer'
    });

    common.extend(value_field.style, {
      width: '100%',
      height: '100%',
      background: 'none'
    });
    
    linearGradient(value_field, 'top', 'rgba(0,0,0,0)', '#000');

    common.extend(this.__hue_field.style, {
      width: '15px',
      height: '100px',
      display: 'inline-block',
      border: '1px solid #555',
      cursor: 'ns-resize'
    });

    hueGradient(this.__hue_field);

    common.extend(this.__input.style, {
      outline: 'none',
//      width: '120px',
      textAlign: 'center',
//      padding: '4px',
//      marginBottom: '6px',
      color: '#fff',
      border: 0,
      fontWeight: 'bold',
      textShadow: this.__input_textShadow + 'rgba(0,0,0,0.7)'
    });

    dom.bind(this.__saturation_field, 'mousedown', fieldDown);
    dom.bind(this.__field_knob, 'mousedown', fieldDown);

    dom.bind(this.__hue_field, 'mousedown', function(e) {
      setH(e);
      dom.bind(window, 'mousemove', setH);
      dom.bind(window, 'mouseup', unbindH);
    });

    function fieldDown(e) {
      setSV(e);
      // document.body.style.cursor = 'none';
      dom.bind(window, 'mousemove', setSV);
      dom.bind(window, 'mouseup', unbindSV);
    }

    function unbindSV() {
      dom.unbind(window, 'mousemove', setSV);
      dom.unbind(window, 'mouseup', unbindSV);
      // document.body.style.cursor = 'default';
    }

    function onBlur() {
      var i = interpret(this.value);
      if (i !== false) {
        _this.__color.__state = i;
        _this.setValue(_this.__color.toOriginal());
      } else {
        this.value = _this.__color.toString();
      }
    }

    function unbindH() {
      dom.unbind(window, 'mousemove', setH);
      dom.unbind(window, 'mouseup', unbindH);
    }

    this.__saturation_field.appendChild(value_field);
    this.__selector.appendChild(this.__field_knob);
    this.__selector.appendChild(this.__saturation_field);
    this.__selector.appendChild(this.__hue_field);
    this.__hue_field.appendChild(this.__hue_knob);

    this.domElement.appendChild(this.__input);
    this.domElement.appendChild(this.__selector);

    this.updateDisplay();

    function setSV(e) {

      e.preventDefault();

      var w = dom.getWidth(_this.__saturation_field);
      var o = dom.getOffset(_this.__saturation_field);
      var s = (e.clientX - o.left + document.body.scrollLeft) / w;
      var v = 1 - (e.clientY - o.top + document.body.scrollTop) / w;

      if (v > 1) v = 1;
      else if (v < 0) v = 0;

      if (s > 1) s = 1;
      else if (s < 0) s = 0;

      _this.__color.v = v;
      _this.__color.s = s;

      _this.setValue(_this.__color.toOriginal());


      return false;

    }

    function setH(e) {

      e.preventDefault();

      var s = dom.getHeight(_this.__hue_field);
      var o = dom.getOffset(_this.__hue_field);
      var h = 1 - (e.clientY - o.top + document.body.scrollTop) / s;

      if (h > 1) h = 1;
      else if (h < 0) h = 0;

      _this.__color.h = h * 360;

      _this.setValue(_this.__color.toOriginal());

      return false;

    }

  };

  ColorController.superclass = Controller;

  common.extend(

      ColorController.prototype,
      Controller.prototype,

      {

        updateDisplay: function() {

          var i = interpret(this.getValue());

          if (i !== false) {

            var mismatch = false;

            // Check for mismatch on the interpreted value.

            common.each(Color.COMPONENTS, function(component) {
              if (!common.isUndefined(i[component]) &&
                  !common.isUndefined(this.__color.__state[component]) &&
                  i[component] !== this.__color.__state[component]) {
                mismatch = true;
                return {}; // break
              }
            }, this);

            // If nothing diverges, we keep our previous values
            // for statefulness, otherwise we recalculate fresh
            if (mismatch) {
              common.extend(this.__color.__state, i);
            }

          }

          common.extend(this.__temp.__state, this.__color.__state);

          this.__temp.a = 1;

          var flip = (this.__color.v < .5 || this.__color.s > .5) ? 255 : 0;
          var _flip = 255 - flip;

          common.extend(this.__field_knob.style, {
            marginLeft: 100 * this.__color.s - 7 + 'px',
            marginTop: 100 * (1 - this.__color.v) - 7 + 'px',
            backgroundColor: this.__temp.toString(),
            border: this.__field_knob_border + 'rgb(' + flip + ',' + flip + ',' + flip +')'
          });

          this.__hue_knob.style.marginTop = (1 - this.__color.h / 360) * 100 + 'px'

          this.__temp.s = 1;
          this.__temp.v = 1;

          linearGradient(this.__saturation_field, 'left', '#fff', this.__temp.toString());

          common.extend(this.__input.style, {
            backgroundColor: this.__input.value = this.__color.toString(),
            color: 'rgb(' + flip + ',' + flip + ',' + flip +')',
            textShadow: this.__input_textShadow + 'rgba(' + _flip + ',' + _flip + ',' + _flip +',.7)'
          });

        }

      }

  );
  
  var vendors = ['-moz-','-o-','-webkit-','-ms-',''];
  
  function linearGradient(elem, x, a, b) {
    elem.style.background = '';
    common.each(vendors, function(vendor) {
      elem.style.cssText += 'background: ' + vendor + 'linear-gradient('+x+', '+a+' 0%, ' + b + ' 100%); ';
    });
  }
  
  function hueGradient(elem) {
    elem.style.background = '';
    elem.style.cssText += 'background: -moz-linear-gradient(top,  #ff0000 0%, #ff00ff 17%, #0000ff 34%, #00ffff 50%, #00ff00 67%, #ffff00 84%, #ff0000 100%);'
    elem.style.cssText += 'background: -webkit-linear-gradient(top,  #ff0000 0%,#ff00ff 17%,#0000ff 34%,#00ffff 50%,#00ff00 67%,#ffff00 84%,#ff0000 100%);'
    elem.style.cssText += 'background: -o-linear-gradient(top,  #ff0000 0%,#ff00ff 17%,#0000ff 34%,#00ffff 50%,#00ff00 67%,#ffff00 84%,#ff0000 100%);'
    elem.style.cssText += 'background: -ms-linear-gradient(top,  #ff0000 0%,#ff00ff 17%,#0000ff 34%,#00ffff 50%,#00ff00 67%,#ffff00 84%,#ff0000 100%);'
    elem.style.cssText += 'background: linear-gradient(top,  #ff0000 0%,#ff00ff 17%,#0000ff 34%,#00ffff 50%,#00ff00 67%,#ffff00 84%,#ff0000 100%);'
  }


  return ColorController;

})(dat.controllers.Controller,
dat.dom.dom,
dat.color.Color = (function (interpret, math, toString, common) {

  var Color = function() {

    this.__state = interpret.apply(this, arguments);

    if (this.__state === false) {
      throw 'Failed to interpret color arguments';
    }

    this.__state.a = this.__state.a || 1;


  };

  Color.COMPONENTS = ['r','g','b','h','s','v','hex','a'];

  common.extend(Color.prototype, {

    toString: function() {
      return toString(this);
    },

    toOriginal: function() {
      return this.__state.conversion.write(this);
    }

  });

  defineRGBComponent(Color.prototype, 'r', 2);
  defineRGBComponent(Color.prototype, 'g', 1);
  defineRGBComponent(Color.prototype, 'b', 0);

  defineHSVComponent(Color.prototype, 'h');
  defineHSVComponent(Color.prototype, 's');
  defineHSVComponent(Color.prototype, 'v');

  Object.defineProperty(Color.prototype, 'a', {

    get: function() {
      return this.__state.a;
    },

    set: function(v) {
      this.__state.a = v;
    }

  });

  Object.defineProperty(Color.prototype, 'hex', {

    get: function() {

      if (!this.__state.space !== 'HEX') {
        this.__state.hex = math.rgb_to_hex(this.r, this.g, this.b);
      }

      return this.__state.hex;

    },

    set: function(v) {

      this.__state.space = 'HEX';
      this.__state.hex = v;

    }

  });

  function defineRGBComponent(target, component, componentHexIndex) {

    Object.defineProperty(target, component, {

      get: function() {

        if (this.__state.space === 'RGB') {
          return this.__state[component];
        }

        recalculateRGB(this, component, componentHexIndex);

        return this.__state[component];

      },

      set: function(v) {

        if (this.__state.space !== 'RGB') {
          recalculateRGB(this, component, componentHexIndex);
          this.__state.space = 'RGB';
        }

        this.__state[component] = v;

      }

    });

  }

  function defineHSVComponent(target, component) {

    Object.defineProperty(target, component, {

      get: function() {

        if (this.__state.space === 'HSV')
          return this.__state[component];

        recalculateHSV(this);

        return this.__state[component];

      },

      set: function(v) {

        if (this.__state.space !== 'HSV') {
          recalculateHSV(this);
          this.__state.space = 'HSV';
        }

        this.__state[component] = v;

      }

    });

  }

  function recalculateRGB(color, component, componentHexIndex) {

    if (color.__state.space === 'HEX') {

      color.__state[component] = math.component_from_hex(color.__state.hex, componentHexIndex);

    } else if (color.__state.space === 'HSV') {

      common.extend(color.__state, math.hsv_to_rgb(color.__state.h, color.__state.s, color.__state.v));

    } else {

      throw 'Corrupted color state';

    }

  }

  function recalculateHSV(color) {

    var result = math.rgb_to_hsv(color.r, color.g, color.b);

    common.extend(color.__state,
        {
          s: result.s,
          v: result.v
        }
    );

    if (!common.isNaN(result.h)) {
      color.__state.h = result.h;
    } else if (common.isUndefined(color.__state.h)) {
      color.__state.h = 0;
    }

  }

  return Color;

})(dat.color.interpret,
dat.color.math = (function () {

  var tmpComponent;

  return {

    hsv_to_rgb: function(h, s, v) {

      var hi = Math.floor(h / 60) % 6;

      var f = h / 60 - Math.floor(h / 60);
      var p = v * (1.0 - s);
      var q = v * (1.0 - (f * s));
      var t = v * (1.0 - ((1.0 - f) * s));
      var c = [
        [v, t, p],
        [q, v, p],
        [p, v, t],
        [p, q, v],
        [t, p, v],
        [v, p, q]
      ][hi];

      return {
        r: c[0] * 255,
        g: c[1] * 255,
        b: c[2] * 255
      };

    },

    rgb_to_hsv: function(r, g, b) {

      var min = Math.min(r, g, b),
          max = Math.max(r, g, b),
          delta = max - min,
          h, s;

      if (max != 0) {
        s = delta / max;
      } else {
        return {
          h: NaN,
          s: 0,
          v: 0
        };
      }

      if (r == max) {
        h = (g - b) / delta;
      } else if (g == max) {
        h = 2 + (b - r) / delta;
      } else {
        h = 4 + (r - g) / delta;
      }
      h /= 6;
      if (h < 0) {
        h += 1;
      }

      return {
        h: h * 360,
        s: s,
        v: max / 255
      };
    },

    rgb_to_hex: function(r, g, b) {
      var hex = this.hex_with_component(0, 2, r);
      hex = this.hex_with_component(hex, 1, g);
      hex = this.hex_with_component(hex, 0, b);
      return hex;
    },

    component_from_hex: function(hex, componentIndex) {
      return (hex >> (componentIndex * 8)) & 0xFF;
    },

    hex_with_component: function(hex, componentIndex, value) {
      return value << (tmpComponent = componentIndex * 8) | (hex & ~ (0xFF << tmpComponent));
    }

  }

})(),
dat.color.toString,
dat.utils.common),
dat.color.interpret,
dat.utils.common),
dat.utils.requestAnimationFrame = (function () {

  /**
   * requirejs version of Paul Irish's RequestAnimationFrame
   * http://paulirish.com/2011/requestanimationframe-for-smart-animating/
   */

  return window.webkitRequestAnimationFrame ||
      window.mozRequestAnimationFrame ||
      window.oRequestAnimationFrame ||
      window.msRequestAnimationFrame ||
      function(callback, element) {

        window.setTimeout(callback, 1000 / 60);

      };
})(),
dat.dom.CenteredDiv = (function (dom, common) {


  var CenteredDiv = function() {

    this.backgroundElement = document.createElement('div');
    common.extend(this.backgroundElement.style, {
      backgroundColor: 'rgba(0,0,0,0.8)',
      top: 0,
      left: 0,
      display: 'none',
      zIndex: '1000',
      opacity: 0,
      WebkitTransition: 'opacity 0.2s linear'
    });

    dom.makeFullscreen(this.backgroundElement);
    this.backgroundElement.style.position = 'fixed';

    this.domElement = document.createElement('div');
    common.extend(this.domElement.style, {
      position: 'fixed',
      display: 'none',
      zIndex: '1001',
      opacity: 0,
      WebkitTransition: '-webkit-transform 0.2s ease-out, opacity 0.2s linear'
    });


    document.body.appendChild(this.backgroundElement);
    document.body.appendChild(this.domElement);

    var _this = this;
    dom.bind(this.backgroundElement, 'click', function() {
      _this.hide();
    });


  };

  CenteredDiv.prototype.show = function() {

    var _this = this;
    


    this.backgroundElement.style.display = 'block';

    this.domElement.style.display = 'block';
    this.domElement.style.opacity = 0;
//    this.domElement.style.top = '52%';
    this.domElement.style.webkitTransform = 'scale(1.1)';

    this.layout();

    common.defer(function() {
      _this.backgroundElement.style.opacity = 1;
      _this.domElement.style.opacity = 1;
      _this.domElement.style.webkitTransform = 'scale(1)';
    });

  };

  CenteredDiv.prototype.hide = function() {

    var _this = this;

    var hide = function() {

      _this.domElement.style.display = 'none';
      _this.backgroundElement.style.display = 'none';

      dom.unbind(_this.domElement, 'webkitTransitionEnd', hide);
      dom.unbind(_this.domElement, 'transitionend', hide);
      dom.unbind(_this.domElement, 'oTransitionEnd', hide);

    };

    dom.bind(this.domElement, 'webkitTransitionEnd', hide);
    dom.bind(this.domElement, 'transitionend', hide);
    dom.bind(this.domElement, 'oTransitionEnd', hide);

    this.backgroundElement.style.opacity = 0;
//    this.domElement.style.top = '48%';
    this.domElement.style.opacity = 0;
    this.domElement.style.webkitTransform = 'scale(1.1)';

  };

  CenteredDiv.prototype.layout = function() {
    this.domElement.style.left = window.innerWidth/2 - dom.getWidth(this.domElement) / 2 + 'px';
    this.domElement.style.top = window.innerHeight/2 - dom.getHeight(this.domElement) / 2 + 'px';
  };
  
  function lockScroll(e) {
    console.log(e);
  }

  return CenteredDiv;

})(dat.dom.dom,
dat.utils.common),
dat.dom.dom,
dat.utils.common);
/*!
 * Paper.js v0.9.21 - The Swiss Army Knife of Vector Graphics Scripting.
 * http://paperjs.org/
 *
 * Copyright (c) 2011 - 2014, Juerg Lehni & Jonathan Puckey
 * http://scratchdisk.com/ & http://jonathanpuckey.com/
 *
 * Distributed under the MIT license. See LICENSE file for details.
 *
 * All rights reserved.
 *
 * Date: Sat Nov 22 09:01:01 2014 -0800
 *
 ***
 *
 * Straps.js - Class inheritance library with support for bean-style accessors
 *
 * Copyright (c) 2006 - 2013 Juerg Lehni
 * http://scratchdisk.com/
 *
 * Distributed under the MIT license.
 *
 ***
 *
 * Acorn.js
 * http://marijnhaverbeke.nl/acorn/
 *
 * Acorn is a tiny, fast JavaScript parser written in JavaScript,
 * created by Marijn Haverbeke and released under an MIT license.
 *
 */
var paper=new function(t){var e=new function(){function n(t,n,i,r,a){function o(s,o){o=o||(o=u(n,s))&&(o.get?o:o.value),"string"==typeof o&&"#"===o[0]&&(o=t[o.substring(1)]||o);var l,d="function"==typeof o,f=o,_=a||d?o&&o.get?s in t:t[s]:null;a&&_||(d&&_&&(o.base=_),d&&r!==!1&&(l=s.match(/^([gs]et|is)(([A-Z])(.*))$/))&&(h[l[3].toLowerCase()+l[4]]=l[2]),f&&!d&&f.get&&"function"==typeof f.get&&e.isPlainObject(f)||(f={value:f,writable:!0}),(u(t,s)||{configurable:!0}).configurable&&(f.configurable=!0,f.enumerable=i),c(t,s,f))}var h={};if(n){for(var l in n)n.hasOwnProperty(l)&&!s.test(l)&&o(l);for(var l in h){var d=h[l],f=t["set"+d],_=t["get"+d]||f&&t["is"+d];!_||r!==!0&&0!==_.length||o(l,{get:_,set:f})}}return t}function i(t,e,n){return t&&("length"in t&&!t.getLength&&"number"==typeof t.length?a:o).call(t,e,n=n||t),n}function r(t,e,n){for(var i in e)!e.hasOwnProperty(i)||n&&n[i]||(t[i]=e[i]);return t}var s=/^(statics|enumerable|beans|preserve)$/,a=[].forEach||function(t,e){for(var n=0,i=this.length;i>n;n++)t.call(e,this[n],n,this)},o=function(t,e){for(var n in this)this.hasOwnProperty(n)&&t.call(e,this[n],n,this)},h=Object.create||function(t){return{__proto__:t}},u=Object.getOwnPropertyDescriptor||function(t,e){var n=t.__lookupGetter__&&t.__lookupGetter__(e);return n?{get:n,set:t.__lookupSetter__(e),enumerable:!0,configurable:!0}:t.hasOwnProperty(e)?{value:t[e],enumerable:!0,configurable:!0,writable:!0}:null},l=Object.defineProperty||function(t,e,n){return(n.get||n.set)&&t.__defineGetter__?(n.get&&t.__defineGetter__(e,n.get),n.set&&t.__defineSetter__(e,n.set)):t[e]=n.value,t},c=function(t,e,n){return delete t[e],l(t,e,n)};return n(function(){for(var t=0,e=arguments.length;e>t;t++)r(this,arguments[t])},{inject:function(t){if(t){var e=t.statics===!0?t:t.statics,i=t.beans,r=t.preserve;e!==t&&n(this.prototype,t,t.enumerable,i,r),n(this,e,!0,i,r)}for(var s=1,a=arguments.length;a>s;s++)this.inject(arguments[s]);return this},extend:function(){for(var t,e=this,i=0,r=arguments.length;r>i&&!(t=arguments[i].initialize);i++);return t=t||function(){e.apply(this,arguments)},t.prototype=h(this.prototype),t.base=e,c(t.prototype,"constructor",{value:t,writable:!0,configurable:!0}),n(t,this,!0),arguments.length?this.inject.apply(t,arguments):t}},!0).inject({inject:function(){for(var t=0,e=arguments.length;e>t;t++){var i=arguments[t];i&&n(this,i,i.enumerable,i.beans,i.preserve)}return this},extend:function(){var t=h(this);return t.inject.apply(t,arguments)},each:function(t,e){return i(this,t,e)},set:function(t){return r(this,t)},clone:function(){return new this.constructor(this)},statics:{each:i,create:h,define:c,describe:u,set:r,clone:function(t){return r(new t.constructor,t)},isPlainObject:function(t){var n=null!=t&&t.constructor;return n&&(n===Object||n===e||"Object"===n.name)},pick:function(){for(var e=0,n=arguments.length;n>e;e++)if(arguments[e]!==t)return arguments[e]}}})};"undefined"!=typeof module&&(module.exports=e),Array.isArray||(Array.isArray=function(t){return"[object Array]"===Object.prototype.toString.call(t)}),document.head||(document.head=document.getElementsByTagName("head")[0]),e.inject({toString:function(){return null!=this._id?(this._class||"Object")+(this._name?" '"+this._name+"'":" @"+this._id):"{ "+e.each(this,function(t,e){if(!/^_/.test(e)){var n=typeof t;this.push(e+": "+("number"===n?a.instance.number(t):"string"===n?"'"+t+"'":t))}},[]).join(", ")+" }"},exportJSON:function(t){return e.exportJSON(this,t)},toJSON:function(){return e.serialize(this)},_set:function(n,i,r){if(n&&(r||e.isPlainObject(n))){var s=n._filtering||n;for(var a in s)if(s.hasOwnProperty(a)&&(!i||!i[a])){var o=n[a];o!==t&&(this[a]=o)}return!0}},statics:{exports:{enumerable:!0},extend:function ne(){var t=ne.base.apply(this,arguments),n=t.prototype._class;return n&&!e.exports[n]&&(e.exports[n]=t),t},equals:function(t,n){function i(t,e){for(var n in t)if(t.hasOwnProperty(n)&&!e.hasOwnProperty(n))return!1;return!0}if(t===n)return!0;if(t&&t.equals)return t.equals(n);if(n&&n.equals)return n.equals(t);if(Array.isArray(t)&&Array.isArray(n)){if(t.length!==n.length)return!1;for(var r=0,s=t.length;s>r;r++)if(!e.equals(t[r],n[r]))return!1;return!0}if(t&&"object"==typeof t&&n&&"object"==typeof n){if(!i(t,n)||!i(n,t))return!1;for(var r in t)if(t.hasOwnProperty(r)&&!e.equals(t[r],n[r]))return!1;return!0}return!1},read:function(n,i,r,s){if(this===e){var a=this.peek(n,i);return n.__index++,a}var o=this.prototype,h=o._readIndex,u=i||h&&n.__index||0;s||(s=n.length-u);var l=n[u];return l instanceof this||r&&r.readNull&&null==l&&1>=s?(h&&(n.__index=u+1),l&&r&&r.clone?l.clone():l):(l=e.create(this.prototype),h&&(l.__read=!0),l=l.initialize.apply(l,u>0||s<n.length?Array.prototype.slice.call(n,u,u+s):n)||l,h&&(n.__index=u+l.__read,l.__read=t),l)},peek:function(t,e){return t[t.__index=e||t.__index||0]},remain:function(t){return t.length-(t.__index||0)},readAll:function(t,e,n){for(var i,r=[],s=e||0,a=t.length;a>s;s++)r.push(Array.isArray(i=t[s])?this.read(i,0,n):this.read(t,s,n,1));return r},readNamed:function(n,i,r,s,a){var o=this.getNamed(n,i),h=o!==t;if(h){var u=n._filtered;u||(u=n._filtered=e.create(n[0]),u._filtering=n[0]),u[i]=t}return this.read(h?[o]:n,r,s,a)},getNamed:function(n,i){var r=n[0];return n._hasObject===t&&(n._hasObject=1===n.length&&e.isPlainObject(r)),n._hasObject?i?r[i]:n._filtered||r:t},hasNamed:function(t,e){return!!this.getNamed(t,e)},isPlainValue:function(t,e){return this.isPlainObject(t)||Array.isArray(t)||e&&"string"==typeof t},serialize:function(t,n,i,r){n=n||{};var s,o=!r;if(o&&(n.formatter=new a(n.precision),r={length:0,definitions:{},references:{},add:function(t,e){var n="#"+t._id,i=this.references[n];if(!i){this.length++;var r=e.call(t),s=t._class;s&&r[0]!==s&&r.unshift(s),this.definitions[n]=r,i=this.references[n]=[n]}return i}}),t&&t._serialize){s=t._serialize(n,r);var h=t._class;!h||i||s._compact||s[0]===h||s.unshift(h)}else if(Array.isArray(t)){s=[];for(var u=0,l=t.length;l>u;u++)s[u]=e.serialize(t[u],n,i,r);i&&(s._compact=!0)}else if(e.isPlainObject(t)){s={};for(var u in t)t.hasOwnProperty(u)&&(s[u]=e.serialize(t[u],n,i,r))}else s="number"==typeof t?n.formatter.number(t,n.precision):t;return o&&r.length>0?[["dictionary",r.definitions],s]:s},deserialize:function(t,n,i){var r=t;if(i=i||{},Array.isArray(t)){var s=t[0],a="dictionary"===s;if(!a){if(i.dictionary&&1==t.length&&/^#/.test(s))return i.dictionary[s];s=e.exports[s]}r=[];for(var o=s?1:0,h=t.length;h>o;o++)r.push(e.deserialize(t[o],n,i));if(a)i.dictionary=r[0];else if(s){var u=r;n?r=n(s,u):(r=e.create(s.prototype),s.apply(r,u))}}else if(e.isPlainObject(t)){r={};for(var l in t)r[l]=e.deserialize(t[l],n,i)}return r},exportJSON:function(t,n){var i=e.serialize(t,n);return n&&n.asString===!1?i:JSON.stringify(i)},importJSON:function(t,n){return e.deserialize("string"==typeof t?JSON.parse(t):t,function(t,i){var r=n&&n.constructor===t?n:e.create(t.prototype),s=r===n;if(1===i.length&&r instanceof y&&(s||!(r instanceof x))){var a=i[0];e.isPlainObject(a)&&(a.insert=!1)}return t.apply(r,i),s&&(n=null),r})},splice:function(e,n,i,r){var s=n&&n.length,a=i===t;i=a?e.length:i,i>e.length&&(i=e.length);for(var o=0;s>o;o++)n[o]._index=i+o;if(a)return e.push.apply(e,n),[];var h=[i,r];n&&h.push.apply(h,n);for(var u=e.splice.apply(e,h),o=0,l=u.length;l>o;o++)u[o]._index=t;for(var o=i+s,l=e.length;l>o;o++)e[o]._index=o;return u},capitalize:function(t){return t.replace(/\b[a-z]/g,function(t){return t.toUpperCase()})},camelize:function(t){return t.replace(/-(.)/g,function(t,e){return e.toUpperCase()})},hyphenate:function(t){return t.replace(/([a-z])([A-Z])/g,"$1-$2").toLowerCase()}}});var n={on:function(t,n){if("string"!=typeof t)e.each(t,function(t,e){this.on(e,t)},this);else{var i=this._eventTypes[t];if(i){var r=this._callbacks=this._callbacks||{};r=r[t]=r[t]||[],-1===r.indexOf(n)&&(r.push(n),i.install&&1==r.length&&i.install.call(this,t))}}return this},off:function(n,i){if("string"!=typeof n)return e.each(n,function(t,e){this.off(e,t)},this),t;var r,s=this._eventTypes[n],a=this._callbacks&&this._callbacks[n];return s&&a&&(!i||-1!==(r=a.indexOf(i))&&1===a.length?(s.uninstall&&s.uninstall.call(this,n),delete this._callbacks[n]):-1!==r&&a.splice(r,1)),this},once:function(t,e){return this.on(t,function(){e.apply(this,arguments),this.off(t,e)})},emit:function(t,e){var n=this._callbacks&&this._callbacks[t];if(!n)return!1;for(var i=[].slice.call(arguments,1),r=0,s=n.length;s>r;r++)if(n[r].apply(this,i)===!1&&e&&e.stop){e.stop();break}return!0},responds:function(t){return!(!this._callbacks||!this._callbacks[t])},attach:"#on",detach:"#off",fire:"#emit",_installEvents:function(t){var e=this._callbacks,n=t?"install":"uninstall";for(var i in e)if(e[i].length>0){var r=this._eventTypes[i],s=r[n];s&&s.call(this,i)}},statics:{inject:function ie(t){var n=t._events;if(n){var i={};e.each(n,function(n,r){var s="string"==typeof n,a=s?n:r,o=e.capitalize(a),h=a.substring(2).toLowerCase();i[h]=s?{}:n,a="_"+a,t["get"+o]=function(){return this[a]},t["set"+o]=function(t){var e=this[a];e&&this.off(h,e),t&&this.on(h,t),this[a]=t}}),t._eventTypes=i}return ie.base.apply(this,arguments)}}},r=e.extend({_class:"PaperScope",initialize:function re(){paper=this,this.settings=new e({applyMatrix:!0,handleSize:4,hitTolerance:0}),this.project=null,this.projects=[],this.tools=[],this.palettes=[],this._id=re._id++,re._scopes[this._id]=this;var t=re.prototype;if(!this.support){var n=K.getContext(1,1);t.support={nativeDash:"setLineDash"in n||"mozDash"in n,nativeBlendModes:Q.nativeModes},K.release(n)}if(!this.browser){var i=t.browser={};navigator.userAgent.toLowerCase().replace(/(opera|chrome|safari|webkit|firefox|msie|trident)\/?\s*([.\d]+)(?:.*version\/([.\d]+))?(?:.*rv\:([.\d]+))?/g,function(t,e,n,r,s){if(!i.chrome){var a="opera"===e?r:n;"trident"===e&&(a=s,e="msie"),i.version=a,i.versionNumber=parseFloat(a),i.name=e,i[e]=!0}}),i.chrome&&delete i.webkit}},version:"0.9.21",getView:function(){return this.project&&this.project.getView()},getPaper:function(){return this},execute:function(t,e,n){paper.PaperScript.execute(t,this,e,n),Z.updateFocus()},install:function(t){var n=this;e.each(["project","view","tool"],function(i){e.define(t,i,{configurable:!0,get:function(){return n[i]}})});for(var i in this)!/^_/.test(i)&&this[i]&&(t[i]=this[i])},setup:function(t){return paper=this,this.project=new v(t),this},activate:function(){paper=this},clear:function(){for(var t=this.projects.length-1;t>=0;t--)this.projects[t].remove();for(var t=this.tools.length-1;t>=0;t--)this.tools[t].remove();for(var t=this.palettes.length-1;t>=0;t--)this.palettes[t].remove()},remove:function(){this.clear(),delete r._scopes[this._id]},statics:new function(){function t(t){return t+="Attribute",function(e,n){return e[t](n)||e[t]("data-paper-"+n)}}return{_scopes:{},_id:0,get:function(t){return this._scopes[t]||null},getAttribute:t("get"),hasAttribute:t("has")}}}),s=e.extend(n,{initialize:function(t){this._scope=paper,this._index=this._scope[this._list].push(this)-1,(t||!this._scope[this._reference])&&this.activate()},activate:function(){if(!this._scope)return!1;var t=this._scope[this._reference];return t&&t!==this&&t.emit("deactivate"),this._scope[this._reference]=this,this.emit("activate",t),!0},isActive:function(){return this._scope[this._reference]===this},remove:function(){return null==this._index?!1:(e.splice(this._scope[this._list],null,this._index,1),this._scope[this._reference]==this&&(this._scope[this._reference]=null),this._scope=null,!0)}}),a=e.extend({initialize:function(t){this.precision=t||5,this.multiplier=Math.pow(10,this.precision)},number:function(t){return Math.round(t*this.multiplier)/this.multiplier},pair:function(t,e,n){return this.number(t)+(n||",")+this.number(e)},point:function(t,e){return this.number(t.x)+(e||",")+this.number(t.y)},size:function(t,e){return this.number(t.width)+(e||",")+this.number(t.height)},rectangle:function(t,e){return this.point(t,e)+(e||",")+this.size(t,e)}});a.instance=new a;var o=new function(){function e(e,n,i){var r=n===t,s=n-c,a=i+c,o=0;return function(t){return(r||t>s&&a>t)&&(e[o++]=n>t?n:t>i?i:t),o}}var n=[[.5773502691896257],[0,.7745966692414834],[.33998104358485626,.8611363115940526],[0,.5384693101056831,.906179845938664],[.2386191860831969,.6612093864662645,.932469514203152],[0,.4058451513773972,.7415311855993945,.9491079123427585],[.1834346424956498,.525532409916329,.7966664774136267,.9602898564975363],[0,.3242534234038089,.6133714327005904,.8360311073266358,.9681602395076261],[.14887433898163122,.4333953941292472,.6794095682990244,.8650633666889845,.9739065285171717],[0,.26954315595234496,.5190961292068118,.7301520055740494,.8870625997680953,.978228658146057],[.1252334085114689,.3678314989981802,.5873179542866175,.7699026741943047,.9041172563704749,.9815606342467192],[0,.2304583159551348,.44849275103644687,.6423493394403402,.8015780907333099,.9175983992229779,.9841830547185881],[.10805494870734367,.31911236892788974,.5152486363581541,.6872929048116855,.827201315069765,.9284348836635735,.9862838086968123],[0,.20119409399743451,.3941513470775634,.5709721726085388,.7244177313601701,.8482065834104272,.937273392400706,.9879925180204854],[.09501250983763744,.2816035507792589,.45801677765722737,.6178762444026438,.755404408355003,.8656312023878318,.9445750230732326,.9894009349916499]],i=[[1],[.8888888888888888,.5555555555555556],[.6521451548625461,.34785484513745385],[.5688888888888889,.47862867049936647,.23692688505618908],[.46791393457269104,.3607615730481386,.17132449237917036],[.4179591836734694,.3818300505051189,.27970539148927664,.1294849661688697],[.362683783378362,.31370664587788727,.22238103445337448,.10122853629037626],[.3302393550012598,.31234707704000286,.26061069640293544,.1806481606948574,.08127438836157441],[.29552422471475287,.26926671930999635,.21908636251598204,.1494513491505806,.06667134430868814],[.2729250867779006,.26280454451024665,.23319376459199048,.18629021092773426,.1255803694649046,.05566856711617366],[.24914704581340277,.2334925365383548,.20316742672306592,.16007832854334622,.10693932599531843,.04717533638651183],[.2325515532308739,.22628318026289723,.2078160475368885,.17814598076194574,.13887351021978725,.09212149983772845,.04048400476531588],[.2152638534631578,.2051984637212956,.18553839747793782,.15720316715819355,.12151857068790319,.08015808715976021,.03511946033175186],[.2025782419255613,.19843148532711158,.1861610000155622,.16626920581699392,.13957067792615432,.10715922046717194,.07036604748810812,.03075324199611727],[.1894506104550685,.18260341504492358,.16915651939500254,.14959598881657674,.12462897125553388,.09515851168249279,.062253523938647894,.027152459411754096]],r=Math.abs,s=Math.sqrt,a=Math.pow,h=Math.cos,u=Math.PI,l=1e-5,c=1e-11;return{TOLERANCE:l,EPSILON:c,KAPPA:4*(s(2)-1)/3,isZero:function(t){return r(t)<=c},integrate:function(t,e,r,s){for(var a=n[s-2],o=i[s-2],h=.5*(r-e),u=h+e,l=0,c=s+1>>1,d=1&s?o[l++]*t(u):0;c>l;){var f=h*a[l];d+=o[l++]*(t(u+f)+t(u-f))}return h*d},findRoot:function(t,e,n,i,s,a,o){for(var h=0;a>h;h++){var u=t(n),l=u/e(n),c=n-l;if(r(l)<o)return c;u>0?(s=n,n=i>=c?.5*(i+s):c):(i=n,n=c>=s?.5*(i+s):c)}return n},solveQuadratic:function(t,n,i,a,o,h){var u=e(a,o,h);if(r(t)<c)return r(n)>=c?u(-i/n):r(i)<c?-1:0;var l=n/(2*t),d=i/t,f=l*l;if(d-c>f)return 0;var _=f>d?s(f-d):0,g=u(_-l);return _>0&&(g=u(-_-l)),g},solveCubic:function(t,n,i,l,d,f,_){if(r(t)<c)return o.solveQuadratic(n,i,l,d,f,_);n/=t,i/=t,l/=t;var g=e(d,f,_),p=n*n,v=(p-3*i)/9,m=(2*p*n-9*n*i+27*l)/54,y=v*v*v,w=m*m-y;if(n/=3,r(w)<c){if(r(m)<c)return g(-n);var x=s(v),b=m>0?1:-1;return g(2*-b*x-n),g(b*x-n)}if(0>w){var x=s(v),C=Math.acos(m/(x*x*x))/3,S=-2*x,P=2*u/3;return g(S*h(C)-n),g(S*h(C+P)-n),g(S*h(C-P)-n)}var k=(m>0?-1:1)*a(r(m)+s(w),1/3);return g(k+v/k-n)}}},h=e.extend({_class:"Point",_readIndex:!0,initialize:function(t,e){var n=typeof t;if("number"===n){var i="number"==typeof e;this.x=t,this.y=i?e:t,this.__read&&(this.__read=i?2:1)}else"undefined"===n||null===t?(this.x=this.y=0,this.__read&&(this.__read=null===t?1:0)):(Array.isArray(t)?(this.x=t[0],this.y=t.length>1?t[1]:t[0]):null!=t.x?(this.x=t.x,this.y=t.y):null!=t.width?(this.x=t.width,this.y=t.height):null!=t.angle?(this.x=t.length,this.y=0,this.setAngle(t.angle)):(this.x=this.y=0,this.__read&&(this.__read=0)),this.__read&&(this.__read=1))},set:function(t,e){return this.x=t,this.y=e,this},equals:function(t){return this===t||t&&(this.x===t.x&&this.y===t.y||Array.isArray(t)&&this.x===t[0]&&this.y===t[1])||!1},clone:function(){return new h(this.x,this.y)},toString:function(){var t=a.instance;return"{ x: "+t.number(this.x)+", y: "+t.number(this.y)+" }"},_serialize:function(t){var e=t.formatter;return[e.number(this.x),e.number(this.y)]},getLength:function(){return Math.sqrt(this.x*this.x+this.y*this.y)},setLength:function(t){if(this.isZero()){var e=this._angle||0;this.set(Math.cos(e)*t,Math.sin(e)*t)}else{var n=t/this.getLength();o.isZero(n)&&this.getAngle(),this.set(this.x*n,this.y*n)}},getAngle:function(){return 180*this.getAngleInRadians.apply(this,arguments)/Math.PI},setAngle:function(t){this.setAngleInRadians.call(this,t*Math.PI/180)},getAngleInDegrees:"#getAngle",setAngleInDegrees:"#setAngle",getAngleInRadians:function(){if(arguments.length){var t=h.read(arguments),e=this.getLength()*t.getLength();if(o.isZero(e))return 0/0;var n=this.dot(t)/e;return Math.acos(-1>n?-1:n>1?1:n)}return this.isZero()?this._angle||0:this._angle=Math.atan2(this.y,this.x)},setAngleInRadians:function(t){if(this._angle=t,!this.isZero()){var e=this.getLength();this.set(Math.cos(t)*e,Math.sin(t)*e)}},getQuadrant:function(){return this.x>=0?this.y>=0?1:4:this.y>=0?2:3}},{beans:!1,getDirectedAngle:function(){var t=h.read(arguments);return 180*Math.atan2(this.cross(t),this.dot(t))/Math.PI},getDistance:function(){var t=h.read(arguments),n=t.x-this.x,i=t.y-this.y,r=n*n+i*i,s=e.read(arguments);return s?r:Math.sqrt(r)},normalize:function(e){e===t&&(e=1);var n=this.getLength(),i=0!==n?e/n:0,r=new h(this.x*i,this.y*i);return i>=0&&(r._angle=this._angle),r},rotate:function(t,e){if(0===t)return this.clone();t=t*Math.PI/180;var n=e?this.subtract(e):this,i=Math.sin(t),r=Math.cos(t);return n=new h(n.x*r-n.y*i,n.x*i+n.y*r),e?n.add(e):n},transform:function(t){return t?t._transformPoint(this):this},add:function(){var t=h.read(arguments);return new h(this.x+t.x,this.y+t.y)},subtract:function(){var t=h.read(arguments);return new h(this.x-t.x,this.y-t.y)},multiply:function(){var t=h.read(arguments);return new h(this.x*t.x,this.y*t.y)},divide:function(){var t=h.read(arguments);return new h(this.x/t.x,this.y/t.y)},modulo:function(){var t=h.read(arguments);return new h(this.x%t.x,this.y%t.y)},negate:function(){return new h(-this.x,-this.y)},isInside:function(){return f.read(arguments).contains(this)},isClose:function(t,e){return this.getDistance(t)<e},isColinear:function(t){return Math.abs(this.cross(t))<1e-5},isOrthogonal:function(t){return Math.abs(this.dot(t))<1e-5},isZero:function(){return o.isZero(this.x)&&o.isZero(this.y)},isNaN:function(){return isNaN(this.x)||isNaN(this.y)},dot:function(){var t=h.read(arguments);return this.x*t.x+this.y*t.y},cross:function(){var t=h.read(arguments);return this.x*t.y-this.y*t.x},project:function(){var t=h.read(arguments);if(t.isZero())return new h(0,0);var e=this.dot(t)/t.dot(t);return new h(t.x*e,t.y*e)},statics:{min:function(){var t=h.read(arguments),e=h.read(arguments);return new h(Math.min(t.x,e.x),Math.min(t.y,e.y))},max:function(){var t=h.read(arguments),e=h.read(arguments);return new h(Math.max(t.x,e.x),Math.max(t.y,e.y))},random:function(){return new h(Math.random(),Math.random())}}},e.each(["round","ceil","floor","abs"],function(t){var e=Math[t];this[t]=function(){return new h(e(this.x),e(this.y))}},{})),u=h.extend({initialize:function(t,e,n,i){this._x=t,this._y=e,this._owner=n,this._setter=i},set:function(t,e,n){return this._x=t,this._y=e,n||this._owner[this._setter](this),this},getX:function(){return this._x},setX:function(t){this._x=t,this._owner[this._setter](this)},getY:function(){return this._y},setY:function(t){this._y=t,this._owner[this._setter](this)}}),c=e.extend({_class:"Size",_readIndex:!0,initialize:function(t,e){var n=typeof t;if("number"===n){var i="number"==typeof e;this.width=t,this.height=i?e:t,this.__read&&(this.__read=i?2:1)}else"undefined"===n||null===t?(this.width=this.height=0,this.__read&&(this.__read=null===t?1:0)):(Array.isArray(t)?(this.width=t[0],this.height=t.length>1?t[1]:t[0]):null!=t.width?(this.width=t.width,this.height=t.height):null!=t.x?(this.width=t.x,this.height=t.y):(this.width=this.height=0,this.__read&&(this.__read=0)),this.__read&&(this.__read=1))},set:function(t,e){return this.width=t,this.height=e,this},equals:function(t){return t===this||t&&(this.width===t.width&&this.height===t.height||Array.isArray(t)&&this.width===t[0]&&this.height===t[1])||!1},clone:function(){return new c(this.width,this.height)},toString:function(){var t=a.instance;return"{ width: "+t.number(this.width)+", height: "+t.number(this.height)+" }"},_serialize:function(t){var e=t.formatter;return[e.number(this.width),e.number(this.height)]},add:function(){var t=c.read(arguments);return new c(this.width+t.width,this.height+t.height)},subtract:function(){var t=c.read(arguments);return new c(this.width-t.width,this.height-t.height)},multiply:function(){var t=c.read(arguments);return new c(this.width*t.width,this.height*t.height)},divide:function(){var t=c.read(arguments);return new c(this.width/t.width,this.height/t.height)},modulo:function(){var t=c.read(arguments);return new c(this.width%t.width,this.height%t.height)},negate:function(){return new c(-this.width,-this.height)},isZero:function(){return o.isZero(this.width)&&o.isZero(this.height)},isNaN:function(){return isNaN(this.width)||isNaN(this.height)},statics:{min:function(t,e){return new c(Math.min(t.width,e.width),Math.min(t.height,e.height))},max:function(t,e){return new c(Math.max(t.width,e.width),Math.max(t.height,e.height))},random:function(){return new c(Math.random(),Math.random())}}},e.each(["round","ceil","floor","abs"],function(t){var e=Math[t];this[t]=function(){return new c(e(this.width),e(this.height))}},{})),d=c.extend({initialize:function(t,e,n,i){this._width=t,this._height=e,this._owner=n,this._setter=i},set:function(t,e,n){return this._width=t,this._height=e,n||this._owner[this._setter](this),this},getWidth:function(){return this._width},setWidth:function(t){this._width=t,this._owner[this._setter](this)},getHeight:function(){return this._height},setHeight:function(t){this._height=t,this._owner[this._setter](this)}}),f=e.extend({_class:"Rectangle",_readIndex:!0,beans:!0,initialize:function(n,i,r,s){var a=typeof n,o=0;if("number"===a?(this.x=n,this.y=i,this.width=r,this.height=s,o=4):"undefined"===a||null===n?(this.x=this.y=this.width=this.height=0,o=null===n?1:0):1===arguments.length&&(Array.isArray(n)?(this.x=n[0],this.y=n[1],this.width=n[2],this.height=n[3],o=1):n.x!==t||n.width!==t?(this.x=n.x||0,this.y=n.y||0,this.width=n.width||0,this.height=n.height||0,o=1):n.from===t&&n.to===t&&(this.x=this.y=this.width=this.height=0,this._set(n),o=1)),!o){var u=h.readNamed(arguments,"from"),l=e.peek(arguments);if(this.x=u.x,this.y=u.y,l&&l.x!==t||e.hasNamed(arguments,"to")){var d=h.readNamed(arguments,"to");this.width=d.x-u.x,this.height=d.y-u.y,this.width<0&&(this.x=d.x,this.width=-this.width),this.height<0&&(this.y=d.y,this.height=-this.height)}else{var f=c.read(arguments);this.width=f.width,this.height=f.height}o=arguments.__index}this.__read&&(this.__read=o)},set:function(t,e,n,i){return this.x=t,this.y=e,this.width=n,this.height=i,this},clone:function(){return new f(this.x,this.y,this.width,this.height)},equals:function(t){var n=e.isPlainValue(t)?f.read(arguments):t;return n===this||n&&this.x===n.x&&this.y===n.y&&this.width===n.width&&this.height===n.height||!1},toString:function(){var t=a.instance;return"{ x: "+t.number(this.x)+", y: "+t.number(this.y)+", width: "+t.number(this.width)+", height: "+t.number(this.height)+" }"},_serialize:function(t){var e=t.formatter;return[e.number(this.x),e.number(this.y),e.number(this.width),e.number(this.height)]},getPoint:function(t){var e=t?h:u;return new e(this.x,this.y,this,"setPoint")},setPoint:function(){var t=h.read(arguments);this.x=t.x,this.y=t.y},getSize:function(t){var e=t?c:d;return new e(this.width,this.height,this,"setSize")},setSize:function(){var t=c.read(arguments);this._fixX&&(this.x+=(this.width-t.width)*this._fixX),this._fixY&&(this.y+=(this.height-t.height)*this._fixY),this.width=t.width,this.height=t.height,this._fixW=1,this._fixH=1},getLeft:function(){return this.x},setLeft:function(t){this._fixW||(this.width-=t-this.x),this.x=t,this._fixX=0},getTop:function(){return this.y},setTop:function(t){this._fixH||(this.height-=t-this.y),this.y=t,this._fixY=0},getRight:function(){return this.x+this.width},setRight:function(e){this._fixX!==t&&1!==this._fixX&&(this._fixW=0),this._fixW?this.x=e-this.width:this.width=e-this.x,this._fixX=1},getBottom:function(){return this.y+this.height},setBottom:function(e){this._fixY!==t&&1!==this._fixY&&(this._fixH=0),this._fixH?this.y=e-this.height:this.height=e-this.y,this._fixY=1},getCenterX:function(){return this.x+.5*this.width},setCenterX:function(t){this.x=t-.5*this.width,this._fixX=.5},getCenterY:function(){return this.y+.5*this.height},setCenterY:function(t){this.y=t-.5*this.height,this._fixY=.5},getCenter:function(t){var e=t?h:u;return new e(this.getCenterX(),this.getCenterY(),this,"setCenter")},setCenter:function(){var t=h.read(arguments);return this.setCenterX(t.x),this.setCenterY(t.y),this},getArea:function(){return this.width*this.height},isEmpty:function(){return 0===this.width||0===this.height},contains:function(e){return e&&e.width!==t||4==(Array.isArray(e)?e:arguments).length?this._containsRectangle(f.read(arguments)):this._containsPoint(h.read(arguments))},_containsPoint:function(t){var e=t.x,n=t.y;return e>=this.x&&n>=this.y&&e<=this.x+this.width&&n<=this.y+this.height},_containsRectangle:function(t){var e=t.x,n=t.y;return e>=this.x&&n>=this.y&&e+t.width<=this.x+this.width&&n+t.height<=this.y+this.height},intersects:function(){var t=f.read(arguments);return t.x+t.width>this.x&&t.y+t.height>this.y&&t.x<this.x+this.width&&t.y<this.y+this.height},touches:function(){var t=f.read(arguments);return t.x+t.width>=this.x&&t.y+t.height>=this.y&&t.x<=this.x+this.width&&t.y<=this.y+this.height},intersect:function(){var t=f.read(arguments),e=Math.max(this.x,t.x),n=Math.max(this.y,t.y),i=Math.min(this.x+this.width,t.x+t.width),r=Math.min(this.y+this.height,t.y+t.height);return new f(e,n,i-e,r-n)},unite:function(){var t=f.read(arguments),e=Math.min(this.x,t.x),n=Math.min(this.y,t.y),i=Math.max(this.x+this.width,t.x+t.width),r=Math.max(this.y+this.height,t.y+t.height);return new f(e,n,i-e,r-n)},include:function(){var t=h.read(arguments),e=Math.min(this.x,t.x),n=Math.min(this.y,t.y),i=Math.max(this.x+this.width,t.x),r=Math.max(this.y+this.height,t.y);return new f(e,n,i-e,r-n)},expand:function(){var t=c.read(arguments),e=t.width,n=t.height;return new f(this.x-e/2,this.y-n/2,this.width+e,this.height+n)},scale:function(e,n){return this.expand(this.width*e-this.width,this.height*(n===t?e:n)-this.height)}},e.each([["Top","Left"],["Top","Right"],["Bottom","Left"],["Bottom","Right"],["Left","Center"],["Top","Center"],["Right","Center"],["Bottom","Center"]],function(t,e){var n=t.join(""),i=/^[RL]/.test(n);e>=4&&(t[1]+=i?"Y":"X");var r=t[i?0:1],s=t[i?1:0],a="get"+r,o="get"+s,l="set"+r,c="set"+s,d="get"+n,f="set"+n;this[d]=function(t){var e=t?h:u;return new e(this[a](),this[o](),this,f)},this[f]=function(){var t=h.read(arguments);this[l](t.x),this[c](t.y)}},{beans:!0})),_=f.extend({initialize:function(t,e,n,i,r,s){this.set(t,e,n,i,!0),this._owner=r,this._setter=s},set:function(t,e,n,i,r){return this._x=t,this._y=e,this._width=n,this._height=i,r||this._owner[this._setter](this),this}},new function(){var t=f.prototype;return e.each(["x","y","width","height"],function(t){var n=e.capitalize(t),i="_"+t;this["get"+n]=function(){return this[i]},this["set"+n]=function(t){this[i]=t,this._dontNotify||this._owner[this._setter](this)}},e.each(["Point","Size","Center","Left","Top","Right","Bottom","CenterX","CenterY","TopLeft","TopRight","BottomLeft","BottomRight","LeftCenter","TopCenter","RightCenter","BottomCenter"],function(e){var n="set"+e;this[n]=function(){this._dontNotify=!0,t[n].apply(this,arguments),this._dontNotify=!1,this._owner[this._setter](this)}},{isSelected:function(){return this._owner._boundsSelected},setSelected:function(t){var e=this._owner;e.setSelected&&(e._boundsSelected=t,e.setSelected(t||e._selectedSegmentState>0))}}))}),g=e.extend({_class:"Matrix",initialize:function se(t){var e=arguments.length,n=!0;if(6===e?this.set.apply(this,arguments):1===e?t instanceof se?this.set(t._a,t._c,t._b,t._d,t._tx,t._ty):Array.isArray(t)?this.set.apply(this,t):n=!1:0===e?this.reset():n=!1,!n)throw Error("Unsupported matrix parameters")},set:function(t,e,n,i,r,s,a){return this._a=t,this._c=e,this._b=n,this._d=i,this._tx=r,this._ty=s,a||this._changed(),this},_serialize:function(t){return e.serialize(this.getValues(),t)},_changed:function(){var t=this._owner;t&&(t._applyMatrix?t.transform(null,!0):t._changed(9))},clone:function(){return new g(this._a,this._c,this._b,this._d,this._tx,this._ty)},equals:function(t){return t===this||t&&this._a===t._a&&this._b===t._b&&this._c===t._c&&this._d===t._d&&this._tx===t._tx&&this._ty===t._ty||!1},toString:function(){var t=a.instance;return"[["+[t.number(this._a),t.number(this._b),t.number(this._tx)].join(", ")+"], ["+[t.number(this._c),t.number(this._d),t.number(this._ty)].join(", ")+"]]"},reset:function(t){return this._a=this._d=1,this._c=this._b=this._tx=this._ty=0,t||this._changed(),this},apply:function(){var t=this._owner;return t?(t.transform(null,!0),this.isIdentity()):!1},translate:function(){var t=h.read(arguments),e=t.x,n=t.y;return this._tx+=e*this._a+n*this._b,this._ty+=e*this._c+n*this._d,this._changed(),this},scale:function(){var t=h.read(arguments),e=h.read(arguments,0,{readNull:!0});return e&&this.translate(e),this._a*=t.x,this._c*=t.x,this._b*=t.y,this._d*=t.y,e&&this.translate(e.negate()),this._changed(),this},rotate:function(t){t*=Math.PI/180;var e=h.read(arguments,1),n=e.x,i=e.y,r=Math.cos(t),s=Math.sin(t),a=n-n*r+i*s,o=i-n*s-i*r,u=this._a,l=this._b,c=this._c,d=this._d;return this._a=r*u+s*l,this._b=-s*u+r*l,this._c=r*c+s*d,this._d=-s*c+r*d,this._tx+=a*u+o*l,this._ty+=a*c+o*d,this._changed(),this},shear:function(){var t=h.read(arguments),e=h.read(arguments,0,{readNull:!0});e&&this.translate(e);var n=this._a,i=this._c;return this._a+=t.y*this._b,this._c+=t.y*this._d,this._b+=t.x*n,this._d+=t.x*i,e&&this.translate(e.negate()),this._changed(),this},skew:function(){var t=h.read(arguments),e=h.read(arguments,0,{readNull:!0}),n=Math.PI/180,i=new h(Math.tan(t.x*n),Math.tan(t.y*n));return this.shear(i,e)},concatenate:function(t){var e=this._a,n=this._b,i=this._c,r=this._d,s=t._a,a=t._b,o=t._c,h=t._d,u=t._tx,l=t._ty;return this._a=s*e+o*n,this._b=a*e+h*n,this._c=s*i+o*r,this._d=a*i+h*r,this._tx+=u*e+l*n,this._ty+=u*i+l*r,this._changed(),this},preConcatenate:function(t){var e=this._a,n=this._b,i=this._c,r=this._d,s=this._tx,a=this._ty,o=t._a,h=t._b,u=t._c,l=t._d,c=t._tx,d=t._ty;return this._a=o*e+h*i,this._b=o*n+h*r,this._c=u*e+l*i,this._d=u*n+l*r,this._tx=o*s+h*a+c,this._ty=u*s+l*a+d,this._changed(),this},chain:function(t){var e=this._a,n=this._b,i=this._c,r=this._d,s=this._tx,a=this._ty,o=t._a,h=t._b,u=t._c,l=t._d,c=t._tx,d=t._ty;return new g(o*e+u*n,o*i+u*r,h*e+l*n,h*i+l*r,s+c*e+d*n,a+c*i+d*r)},isIdentity:function(){return 1===this._a&&0===this._c&&0===this._b&&1===this._d&&0===this._tx&&0===this._ty},orNullIfIdentity:function(){return this.isIdentity()?null:this},isInvertible:function(){return!!this._getDeterminant()},isSingular:function(){return!this._getDeterminant()},transform:function(t,e,n){return arguments.length<3?this._transformPoint(h.read(arguments)):this._transformCoordinates(t,e,n)
},_transformPoint:function(t,e,n){var i=t.x,r=t.y;return e||(e=new h),e.set(i*this._a+r*this._b+this._tx,i*this._c+r*this._d+this._ty,n)},_transformCoordinates:function(t,e,n){for(var i=0,r=0,s=2*n;s>i;){var a=t[i++],o=t[i++];e[r++]=a*this._a+o*this._b+this._tx,e[r++]=a*this._c+o*this._d+this._ty}return e},_transformCorners:function(t){var e=t.x,n=t.y,i=e+t.width,r=n+t.height,s=[e,n,i,n,i,r,e,r];return this._transformCoordinates(s,s,4)},_transformBounds:function(t,e,n){for(var i=this._transformCorners(t),r=i.slice(0,2),s=i.slice(),a=2;8>a;a++){var o=i[a],h=1&a;o<r[h]?r[h]=o:o>s[h]&&(s[h]=o)}return e||(e=new f),e.set(r[0],r[1],s[0]-r[0],s[1]-r[1],n)},inverseTransform:function(){return this._inverseTransform(h.read(arguments))},_getDeterminant:function(){var t=this._a*this._d-this._b*this._c;return isFinite(t)&&!o.isZero(t)&&isFinite(this._tx)&&isFinite(this._ty)?t:null},_inverseTransform:function(t,e,n){var i=this._getDeterminant();if(!i)return null;var r=t.x-this._tx,s=t.y-this._ty;return e||(e=new h),e.set((r*this._d-s*this._b)/i,(s*this._a-r*this._c)/i,n)},decompose:function(){var t=this._a,e=this._b,n=this._c,i=this._d;if(o.isZero(t*i-e*n))return null;var r=Math.sqrt(t*t+e*e);t/=r,e/=r;var s=t*n+e*i;n-=t*s,i-=e*s;var a=Math.sqrt(n*n+i*i);return n/=a,i/=a,s/=a,e*n>t*i&&(t=-t,e=-e,s=-s,r=-r),{scaling:new h(r,a),rotation:180*-Math.atan2(e,t)/Math.PI,shearing:s}},getValues:function(){return[this._a,this._c,this._b,this._d,this._tx,this._ty]},getTranslation:function(){return new h(this._tx,this._ty)},getScaling:function(){return(this.decompose()||{}).scaling},getRotation:function(){return(this.decompose()||{}).rotation},inverted:function(){var t=this._getDeterminant();return t&&new g(this._d/t,-this._c/t,-this._b/t,this._a/t,(this._b*this._ty-this._d*this._tx)/t,(this._c*this._tx-this._a*this._ty)/t)},shiftless:function(){return new g(this._a,this._c,this._b,this._d,0,0)},applyToContext:function(t){t.transform(this._a,this._c,this._b,this._d,this._tx,this._ty)}},e.each(["a","c","b","d","tx","ty"],function(t){var n=e.capitalize(t),i="_"+t;this["get"+n]=function(){return this[i]},this["set"+n]=function(t){this[i]=t,this._changed()}},{})),p=e.extend({_class:"Line",initialize:function(t,e,n,i,r){var s=!1;arguments.length>=4?(this._px=t,this._py=e,this._vx=n,this._vy=i,s=r):(this._px=t.x,this._py=t.y,this._vx=e.x,this._vy=e.y,s=n),s||(this._vx-=this._px,this._vy-=this._py)},getPoint:function(){return new h(this._px,this._py)},getVector:function(){return new h(this._vx,this._vy)},getLength:function(){return this.getVector().getLength()},intersect:function(t,e){return p.intersect(this._px,this._py,this._vx,this._vy,t._px,t._py,t._vx,t._vy,!0,e)},getSide:function(t){return p.getSide(this._px,this._py,this._vx,this._vy,t.x,t.y,!0)},getDistance:function(t){return Math.abs(p.getSignedDistance(this._px,this._py,this._vx,this._vy,t.x,t.y,!0))},statics:{intersect:function(t,e,n,i,r,s,a,u,l,c){l||(n-=t,i-=e,a-=r,u-=s);var d=n*u-i*a;if(!o.isZero(d)){var f=t-r,_=e-s,g=(a*_-u*f)/d,p=(n*_-i*f)/d;if(c||g>=0&&1>=g&&p>=0&&1>=p)return new h(t+g*n,e+g*i)}},getSide:function(t,e,n,i,r,s,a){a||(n-=t,i-=e);var o=r-t,h=s-e,u=o*i-h*n;return 0===u&&(u=o*n+h*i,u>0&&(o-=n,h-=i,u=o*n+h*i,0>u&&(u=0))),0>u?-1:u>0?1:0},getSignedDistance:function(t,e,n,i,r,s,a){if(a||(n-=t,i-=e),o.isZero(n))return r-t;var h=i/n,u=e-h*t;return(s-h*r-u)/Math.sqrt(h*h+1)}}}),v=s.extend({_class:"Project",_list:"projects",_reference:"project",initialize:function(t){s.call(this,!0),this.layers=[],this._activeLayer=null,this.symbols=[],this._currentStyle=new F(null,null,this),this._view=Z.create(this,t||K.getCanvas(1,1)),this._selectedItems={},this._selectedItemCount=0,this._updateVersion=0},_serialize:function(t,n){return e.serialize(this.layers,t,!0,n)},clear:function(){for(var t=this.layers.length-1;t>=0;t--)this.layers[t].remove();this.symbols=[]},isEmpty:function(){return 0===this.layers.length},remove:function ae(){return ae.base.call(this)?(this._view&&this._view.remove(),!0):!1},getView:function(){return this._view},getCurrentStyle:function(){return this._currentStyle},setCurrentStyle:function(t){this._currentStyle.initialize(t)},getIndex:function(){return this._index},getOptions:function(){return this._scope.settings},getActiveLayer:function(){return this._activeLayer||new x({project:this})},getSelectedItems:function(){var t=[];for(var e in this._selectedItems){var n=this._selectedItems[e];n.isInserted()&&t.push(n)}return t},addChild:function(t){return t instanceof x?(e.splice(this.layers,[t]),this._activeLayer||(this._activeLayer=t)):t instanceof y?(this._activeLayer||this.addChild(new x(y.NO_INSERT))).addChild(t):t=null,t},_updateSelection:function(t){var e=t._id,n=this._selectedItems;t._selected?n[e]!==t&&(this._selectedItemCount++,n[e]=t):n[e]===t&&(this._selectedItemCount--,delete n[e])},selectAll:function(){for(var t=this.layers,e=0,n=t.length;n>e;e++)t[e].setFullySelected(!0)},deselectAll:function(){var t=this._selectedItems;for(var e in t)t[e].setFullySelected(!1)},hitTest:function(){for(var t=h.read(arguments),n=P.getOptions(e.read(arguments)),i=this.layers.length-1;i>=0;i--){var r=this.layers[i]._hitTest(t,n);if(r)return r}return null},getItems:function(t){return y._getItems(this.layers,t)},getItem:function(t){return y._getItems(this.layers,t,null,null,!0)[0]||null},importJSON:function(t){this.activate();var n=this._activeLayer;return e.importJSON(t,n&&n.isEmpty()&&n)},draw:function(t,n,i){this._updateVersion++,t.save(),n.applyToContext(t);for(var r=new e({offset:new h(0,0),pixelRatio:i,viewMatrix:n.isIdentity()?null:n,matrices:[new g],updateMatrix:!0}),s=0,a=this.layers,o=a.length;o>s;s++)a[s].draw(t,r);if(t.restore(),this._selectedItemCount>0){t.save(),t.strokeWidth=1;var u=this._selectedItems,l=this._scope.settings.handleSize,c=this._updateVersion;for(var d in u)u[d]._drawSelection(t,n,l,u,c);t.restore()}}}),m=e.extend({_class:"Symbol",initialize:function oe(t,e){this._id=oe._id=(oe._id||0)+1,this.project=paper.project,this.project.symbols.push(this),t&&this.setDefinition(t,e)},_serialize:function(t,n){return n.add(this,function(){return e.serialize([this._class,this._definition],t,!1,n)})},_changed:function(t){8&t&&y._clearBoundsCache(this),1&t&&(this.project._needsUpdate=!0)},getDefinition:function(){return this._definition},setDefinition:function(t,e){t._parentSymbol&&(t=t.clone()),this._definition&&(this._definition._parentSymbol=null),this._definition=t,t.remove(),t.setSelected(!1),e||t.setPosition(new h),t._parentSymbol=this,this._changed(9)},place:function(t){return new S(this,t)},clone:function(){return new m(this._definition.clone(!1))}}),y=e.extend(n,{statics:{extend:function he(t){return t._serializeFields&&(t._serializeFields=new e(this.prototype._serializeFields,t._serializeFields)),he.base.apply(this,arguments)},NO_INSERT:{insert:!1}},_class:"Item",_applyMatrix:!0,_canApplyMatrix:!0,_boundsSelected:!1,_selectChildren:!1,_serializeFields:{name:null,applyMatrix:null,matrix:new g,pivot:null,locked:!1,visible:!0,blendMode:"normal",opacity:1,guide:!1,selected:!1,clipMask:!1,data:{}},initialize:function(){},_initialize:function(t,n){var i=t&&e.isPlainObject(t),r=i&&t.internal===!0,s=this._matrix=new g,a=i&&t.project||paper.project;return r||(this._id=y._id=(y._id||0)+1),this._applyMatrix=this._canApplyMatrix&&paper.settings.applyMatrix,n&&s.translate(n),s._owner=this,this._style=new F(a._currentStyle,this,a),this._project||(r||i&&t.insert===!1?this._setProject(a):i&&t.parent?this.setParent(t.parent):(a._activeLayer||new x).addChild(this)),i&&t!==y.NO_INSERT&&this._set(t,{insert:!0,parent:!0},!0),i},_events:new function(){var t={mousedown:{mousedown:1,mousedrag:1,click:1,doubleclick:1},mouseup:{mouseup:1,mousedrag:1,click:1,doubleclick:1},mousemove:{mousedrag:1,mousemove:1,mouseenter:1,mouseleave:1}},n={install:function(e){var n=this.getView()._eventCounters;if(n)for(var i in t)n[i]=(n[i]||0)+(t[i][e]||0)},uninstall:function(e){var n=this.getView()._eventCounters;if(n)for(var i in t)n[i]-=t[i][e]||0}};return e.each(["onMouseDown","onMouseUp","onMouseDrag","onClick","onDoubleClick","onMouseMove","onMouseEnter","onMouseLeave"],function(t){this[t]=n},{onFrame:{install:function(){this._animateItem(!0)},uninstall:function(){this._animateItem(!1)}},onLoad:{}})},_animateItem:function(t){this.getView()._animateItem(this,t)},_serialize:function(t,n){function i(i){for(var a in i){var o=s[a];e.equals(o,"leading"===a?1.2*i.fontSize:i[a])||(r[a]=e.serialize(o,t,"data"!==a,n))}}var r={},s=this;return i(this._serializeFields),this instanceof w||i(this._style._defaults),[this._class,r]},_changed:function(e){var n=this._parentSymbol,i=this._parent||n,r=this._project;if(8&e&&(this._bounds=this._position=this._decomposed=this._globalMatrix=this._currentPath=t),i&&40&e&&y._clearBoundsCache(i),2&e&&y._clearBoundsCache(this),r&&(1&e&&(r._needsUpdate=!0),r._changes)){var s=r._changesById[this._id];s?s.flags|=e:(s={item:this,flags:e},r._changesById[this._id]=s,r._changes.push(s))}n&&n._changed(e)},set:function(t){return t&&this._set(t),this},getId:function(){return this._id},getClassName:function(){return this._class},getName:function(){return this._name},setName:function(e,n){if(this._name&&this._removeNamed(),e===+e+"")throw Error("Names consisting only of numbers are not supported.");var i=this._parent;if(e&&i){for(var r=i._children,s=i._namedChildren,a=e,o=1;n&&r[e];)e=a+" "+o++;(s[e]=s[e]||[]).push(this),r[e]=this}this._name=e||t,this._changed(128)},getStyle:function(){return this._style},setStyle:function(t){this.getStyle().set(t)}},e.each(["locked","visible","blendMode","opacity","guide"],function(t){var n=e.capitalize(t),t="_"+t;this["get"+n]=function(){return this[t]},this["set"+n]=function(e){e!=this[t]&&(this[t]=e,this._changed("_locked"===t?128:129))}},{}),{beans:!0,_locked:!1,_visible:!0,_blendMode:"normal",_opacity:1,_guide:!1,isSelected:function(){if(this._selectChildren)for(var t=this._children,e=0,n=t.length;n>e;e++)if(t[e].isSelected())return!0;return this._selected},setSelected:function(t,e){if(!e&&this._selectChildren)for(var n=this._children,i=0,r=n.length;r>i;i++)n[i].setSelected(t);(t=!!t)^this._selected&&(this._selected=t,this._project._updateSelection(this),this._changed(129))},_selected:!1,isFullySelected:function(){var t=this._children;if(t&&this._selected){for(var e=0,n=t.length;n>e;e++)if(!t[e].isFullySelected())return!1;return!0}return this._selected},setFullySelected:function(t){var e=this._children;if(e)for(var n=0,i=e.length;i>n;n++)e[n].setFullySelected(t);this.setSelected(t,!0)},isClipMask:function(){return this._clipMask},setClipMask:function(t){this._clipMask!=(t=!!t)&&(this._clipMask=t,t&&(this.setFillColor(null),this.setStrokeColor(null)),this._changed(129),this._parent&&this._parent._changed(1024))},_clipMask:!1,getData:function(){return this._data||(this._data={}),this._data},setData:function(t){this._data=t},getPosition:function(t){var e=this._position,n=t?h:u;if(!e){var i=this._pivot;e=this._position=i?this._matrix._transformPoint(i):this.getBounds().getCenter(!0)}return new n(e.x,e.y,this,"setPosition")},setPosition:function(){this.translate(h.read(arguments).subtract(this.getPosition(!0)))},getPivot:function(t){var e=this._pivot;if(e){var n=t?h:u;e=new n(e.x,e.y,this,"setPivot")}return e},setPivot:function(){this._pivot=h.read(arguments),this._position=t},_pivot:null,getRegistration:"#getPivot",setRegistration:"#setPivot"},e.each(["bounds","strokeBounds","handleBounds","roughBounds","internalBounds","internalRoughBounds"],function(t){var n="get"+e.capitalize(t),i=t.match(/^internal(.*)$/),r=i?"get"+i[1]:null;this[n]=function(e){var i=this._boundsGetter,s=!r&&("string"==typeof i?i:i&&i[n])||n,a=this._getCachedBounds(s,e,this,r);return"bounds"===t?new _(a.x,a.y,a.width,a.height,this,"setBounds"):a}},{beans:!0,_getBounds:function(t,e,n){var i=this._children;if(!i||0==i.length)return new f;for(var r=1/0,s=-r,a=r,o=s,h=0,u=i.length;u>h;h++){var l=i[h];if(l._visible&&!l.isEmpty()){var c=l._getCachedBounds(t,e&&e.chain(l._matrix),n);r=Math.min(c.x,r),a=Math.min(c.y,a),s=Math.max(c.x+c.width,s),o=Math.max(c.y+c.height,o)}}return isFinite(r)?new f(r,a,s-r,o-a):new f},setBounds:function(){var t=f.read(arguments),e=this.getBounds(),n=new g,i=t.getCenter();n.translate(i),(t.width!=e.width||t.height!=e.height)&&n.scale(0!=e.width?t.width/e.width:1,0!=e.height?t.height/e.height:1),i=e.getCenter(),n.translate(-i.x,-i.y),this.transform(n)},_getCachedBounds:function(t,e,n,i){e=e&&e.orNullIfIdentity();var r=i?null:this._matrix.orNullIfIdentity(),s=(!e||e.equals(r))&&t,a=this._parent||this._parentSymbol;if(a){var o=n._id,h=a._boundsCache=a._boundsCache||{ids:{},list:[]};h.ids[o]||(h.list.push(n),h.ids[o]=n)}if(s&&this._bounds&&this._bounds[s])return this._bounds[s].clone();var u=this._getBounds(i||t,e||r,n);if(s){this._bounds||(this._bounds={});var l=this._bounds[s]=u.clone();l._internal=!!i}return u},statics:{_clearBoundsCache:function(e){var n=e._boundsCache;if(n){e._bounds=e._position=e._boundsCache=t;for(var i=0,r=n.list,s=r.length;s>i;i++){var a=r[i];a!==e&&(a._bounds=a._position=t,a._boundsCache&&y._clearBoundsCache(a))}}}}}),{beans:!0,_decompose:function(){return this._decomposed=this._matrix.decompose()},getRotation:function(){var t=this._decomposed||this._decompose();return t&&t.rotation},setRotation:function(t){var e=this.getRotation();if(null!=e&&null!=t){var n=this._decomposed;this.rotate(t-e),n.rotation=t,this._decomposed=n}},getScaling:function(t){var e=this._decomposed||this._decompose(),n=e&&e.scaling,i=t?h:u;return n&&new i(n.x,n.y,this,"setScaling")},setScaling:function(){var t=this.getScaling();if(t){var e=h.read(arguments,0,{clone:!0}),n=this._decomposed;this.scale(e.x/t.x,e.y/t.y),n.scaling=e,this._decomposed=n}},getMatrix:function(){return this._matrix},setMatrix:function(t){this._matrix.initialize(t),this._applyMatrix?this.transform(null,!0):this._changed(9)},getGlobalMatrix:function(t){var e=this._globalMatrix,n=this._project._updateVersion;if(e&&e._updateVersion!==n&&(e=null),!e){e=this._globalMatrix=this._matrix.clone();var i=this._parent;i&&e.preConcatenate(i.getGlobalMatrix(!0)),e._updateVersion=n}return t?e:e.clone()},getApplyMatrix:function(){return this._applyMatrix},setApplyMatrix:function(t){(this._applyMatrix=this._canApplyMatrix&&!!t)&&this.transform(null,!0)},getTransformContent:"#getApplyMatrix",setTransformContent:"#setApplyMatrix"},{getProject:function(){return this._project},_setProject:function(t,e){if(this._project!==t){this._project&&this._installEvents(!1),this._project=t;for(var n=this._children,i=0,r=n&&n.length;r>i;i++)n[i]._setProject(t);e=!0}e&&this._installEvents(!0)},getView:function(){return this._project.getView()},_installEvents:function ue(t){ue.base.call(this,t);for(var e=this._children,n=0,i=e&&e.length;i>n;n++)e[n]._installEvents(t)},getLayer:function(){for(var t=this;t=t._parent;)if(t instanceof x)return t;return null},getParent:function(){return this._parent},setParent:function(t){return t.addChild(this)},getChildren:function(){return this._children},setChildren:function(t){this.removeChildren(),this.addChildren(t)},getFirstChild:function(){return this._children&&this._children[0]||null},getLastChild:function(){return this._children&&this._children[this._children.length-1]||null},getNextSibling:function(){return this._parent&&this._parent._children[this._index+1]||null},getPreviousSibling:function(){return this._parent&&this._parent._children[this._index-1]||null},getIndex:function(){return this._index},equals:function(t){return t===this||t&&this._class===t._class&&this._style.equals(t._style)&&this._matrix.equals(t._matrix)&&this._locked===t._locked&&this._visible===t._visible&&this._blendMode===t._blendMode&&this._opacity===t._opacity&&this._clipMask===t._clipMask&&this._guide===t._guide&&this._equals(t)||!1},_equals:function(t){return e.equals(this._children,t._children)},clone:function(t){return this._clone(new this.constructor(y.NO_INSERT),t)},_clone:function(n,i){if(n.setStyle(this._style),this._children)for(var r=0,s=this._children.length;s>r;r++)n.addChild(this._children[r].clone(!1),!0);(i||i===t)&&n.insertAbove(this);for(var a=["_locked","_visible","_blendMode","_opacity","_clipMask","_guide","_applyMatrix"],r=0,s=a.length;s>r;r++){var o=a[r];this.hasOwnProperty(o)&&(n[o]=this[o])}return n._matrix.initialize(this._matrix),n._data=this._data?e.clone(this._data):null,n.setSelected(this._selected),this._name&&n.setName(this._name,!0),n},copyTo:function(t){return t.addChild(this.clone(!1))},rasterize:function(t){var n=this.getStrokeBounds(),i=(t||this.getView().getResolution())/72,r=n.getTopLeft().floor(),s=n.getBottomRight().ceil(),a=new c(s.subtract(r)),o=K.getCanvas(a.multiply(i)),h=o.getContext("2d"),u=(new g).scale(i).translate(r.negate());h.save(),u.applyToContext(h),this.draw(h,new e({matrices:[u]})),h.restore();var l=new C(y.NO_INSERT);return l.setCanvas(o),l.transform((new g).translate(r.add(a.divide(2))).scale(1/i)),l.insertAbove(this),l},contains:function(){return!!this._contains(this._matrix._inverseTransform(h.read(arguments)))},_contains:function(t){if(this._children){for(var e=this._children.length-1;e>=0;e--)if(this._children[e].contains(t))return!0;return!1}return t.isInside(this.getInternalBounds())},isInside:function(){return f.read(arguments).contains(this.getBounds())},_asPathItem:function(){return new O.Rectangle({rectangle:this.getInternalBounds(),matrix:this._matrix,insert:!1})},intersects:function(t,e){return t instanceof y?this._asPathItem().getIntersections(t._asPathItem(),e||t._matrix).length>0:!1},hitTest:function(){return this._hitTest(h.read(arguments),P.getOptions(e.read(arguments)))},_hitTest:function(n,i){function r(i,r){var s=_["get"+r]();return n.subtract(s).divide(u).length<=1?new P(i,f,{name:e.hyphenate(r),point:s}):t}if(this._locked||!this._visible||this._guide&&!i.guides||this.isEmpty())return null;var s=this._matrix,a=i._totalMatrix,o=this.getView(),h=i._totalMatrix=a?a.chain(s):this.getGlobalMatrix().preConcatenate(o._matrix),u=i._tolerancePadding=new c(O._getPenPadding(1,h.inverted())).multiply(Math.max(i.tolerance,1e-5));if(n=s._inverseTransform(n),!this._children&&!this.getInternalRoughBounds().expand(u.multiply(2))._containsPoint(n))return null;var l,d=!(i.guides&&!this._guide||i.selected&&!this._selected||i.type&&i.type!==e.hyphenate(this._class)||i.class&&!(this instanceof i.class)),f=this;if(d&&(i.center||i.bounds)&&this._parent){var _=this.getInternalBounds();if(i.center&&(l=r("center","Center")),!l&&i.bounds)for(var g=["TopLeft","TopRight","BottomLeft","BottomRight","LeftCenter","TopCenter","RightCenter","BottomCenter"],p=0;8>p&&!l;p++)l=r("bounds",g[p])}var v=!l&&this._children;if(v)for(var m=this._getChildHitTestOptions(i),p=v.length-1;p>=0&&!l;p--)l=v[p]._hitTest(n,m);return!l&&d&&(l=this._hitTestSelf(n,i)),l&&l.point&&(l.point=s.transform(l.point)),i._totalMatrix=a,l},_getChildHitTestOptions:function(t){return t},_hitTestSelf:function(e,n){return n.fill&&this.hasFill()&&this._contains(e)?new P("fill",this):t},matches:function(t,n){function i(t,n){for(var r in t)if(t.hasOwnProperty(r)){var s=t[r],a=n[r];if(e.isPlainObject(s)&&e.isPlainObject(a)){if(!i(s,a))return!1}else if(!e.equals(s,a))return!1}return!0}if("object"==typeof t){for(var r in t)if(t.hasOwnProperty(r)&&!this.matches(r,t[r]))return!1}else{var s=/^(empty|editable)$/.test(t)?this["is"+e.capitalize(t)]():"type"===t?e.hyphenate(this._class):this[t];if(/^(constructor|class)$/.test(t)){if(!(this instanceof n))return!1}else if(n instanceof RegExp){if(!n.test(s))return!1}else if("function"==typeof n){if(!n(s))return!1}else if(e.isPlainObject(n)){if(!i(n,s))return!1}else if(!e.equals(s,n))return!1}return!0},getItems:function(t){return y._getItems(this._children,t,this._matrix)},getItem:function(t){return y._getItems(this._children,t,this._matrix,null,!0)[0]||null},statics:{_getItems:function le(t,n,i,r,s){if(!r){var a=n.overlapping,o=n.inside,h=a||o,u=h&&f.read([h]);r={items:[],inside:u,overlapping:a&&new O.Rectangle({rectangle:u,insert:!1})},h&&(n=e.set({},n,{inside:!0,overlapping:!0}))}var l=r.items,o=r.inside,a=r.overlapping;i=o&&(i||new g);for(var c=0,d=t&&t.length;d>c;c++){var _=t[c],p=i&&i.chain(_._matrix),v=!0;if(o){var h=_.getBounds(p);if(!o.intersects(h))continue;o&&o.contains(h)||a&&a.intersects(_,p)||(v=!1)}if(v&&_.matches(n)&&(l.push(_),s))break;if(le(_._children,n,p,r,s),s&&l.length>0)break}return l}}},{importJSON:function(t){var n=e.importJSON(t,this);return n!==this?this.addChild(n):n},addChild:function(e,n){return this.insertChild(t,e,n)},insertChild:function(t,e,n){var i=this.insertChildren(t,[e],n);return i&&i[0]},addChildren:function(t,e){return this.insertChildren(this._children.length,t,e)},insertChildren:function(t,n,i,r){var s=this._children;if(s&&n&&n.length>0){n=Array.prototype.slice.apply(n);for(var a=n.length-1;a>=0;a--){var o=n[a];!r||o instanceof r?o._remove(!1,!0):n.splice(a,1)}e.splice(s,n,t,0);for(var h=this._project,u=h&&h._changes,a=0,l=n.length;l>a;a++){var o=n[a];o._parent=this,o._setProject(this._project,!0),o._name&&o.setName(o._name),u&&this._changed(5)}this._changed(11)}else n=null;return n},_insert:function(t,e,n){if(!e._parent)return null;var i=e._index+(t?1:0);return e._parent===this._parent&&i>this._index&&i--,e._parent.insertChild(i,this,n)},insertAbove:function(t,e){return this._insert(!0,t,e)},insertBelow:function(t,e){return this._insert(!1,t,e)},sendToBack:function(){return this._parent.insertChild(0,this)},bringToFront:function(){return this._parent.addChild(this)},appendTop:"#addChild",appendBottom:function(t){return this.insertChild(0,t)},moveAbove:"#insertAbove",moveBelow:"#insertBelow",reduce:function(){if(this._children&&1===this._children.length){var t=this._children[0].reduce();return t.insertAbove(this),t.setStyle(this._style),this.remove(),t}return this},_removeNamed:function(){var t=this._parent;if(t){var e=t._children,n=t._namedChildren,i=this._name,r=n[i],s=r?r.indexOf(this):-1;-1!==s&&(e[i]==this&&delete e[i],r.splice(s,1),r.length?e[i]=r[r.length-1]:delete n[i])}},_remove:function(t,n){var i=this._parent;if(i){if(this._name&&this._removeNamed(),null!=this._index&&e.splice(i._children,null,this._index,1),this._installEvents(!1),t){var r=this._project;r&&r._changes&&this._changed(5)}return n&&i._changed(11),this._parent=null,!0}return!1},remove:function(){return this._remove(!0,!0)},replaceWith:function(t){var e=t&&t.insertBelow(this);return e&&this.remove(),e},removeChildren:function(t,n){if(!this._children)return null;t=t||0,n=e.pick(n,this._children.length);for(var i=e.splice(this._children,null,t,n-t),r=i.length-1;r>=0;r--)i[r]._remove(!0,!1);return i.length>0&&this._changed(11),i},clear:"#removeChildren",reverseChildren:function(){if(this._children){this._children.reverse();for(var t=0,e=this._children.length;e>t;t++)this._children[t]._index=t;this._changed(11)}},isEmpty:function(){return!this._children||0===this._children.length},isEditable:function(){for(var t=this;t;){if(!t._visible||t._locked)return!1;t=t._parent}return!0},hasFill:function(){return this.getStyle().hasFill()},hasStroke:function(){return this.getStyle().hasStroke()},hasShadow:function(){return this.getStyle().hasShadow()},_getOrder:function(t){function e(t){var e=[];do e.unshift(t);while(t=t._parent);return e}for(var n=e(this),i=e(t),r=0,s=Math.min(n.length,i.length);s>r;r++)if(n[r]!=i[r])return n[r]._index<i[r]._index?1:-1;return 0},hasChildren:function(){return this._children&&this._children.length>0},isInserted:function(){return this._parent?this._parent.isInserted():!1},isAbove:function(t){return-1===this._getOrder(t)},isBelow:function(t){return 1===this._getOrder(t)},isParent:function(t){return this._parent===t},isChild:function(t){return t&&t._parent===this},isDescendant:function(t){for(var e=this;e=e._parent;)if(e==t)return!0;return!1},isAncestor:function(t){return t?t.isDescendant(this):!1},isGroupedWith:function(t){for(var e=this._parent;e;){if(e._parent&&/^(Group|Layer|CompoundPath)$/.test(e._class)&&t.isDescendant(e))return!0;e=e._parent}return!1},translate:function(){var t=new g;return this.transform(t.translate.apply(t,arguments))},rotate:function(t){return this.transform((new g).rotate(t,h.read(arguments,1,{readNull:!0})||this.getPosition(!0)))}},e.each(["scale","shear","skew"],function(t){this[t]=function(){var e=h.read(arguments),n=h.read(arguments,0,{readNull:!0});return this.transform((new g)[t](e,n||this.getPosition(!0)))}},{}),{transform:function(t,e){t&&t.isIdentity()&&(t=null);var n=this._matrix,i=(e||this._applyMatrix)&&(!n.isIdentity()||t);if(!t&&!i)return this;if(t&&n.preConcatenate(t),i=i&&this._transformContent(n)){var r=this._pivot,s=this._style,a=s.getFillColor(!0),o=s.getStrokeColor(!0);r&&n._transformPoint(r,r,!0),a&&a.transform(n),o&&o.transform(n),n.reset(!0)}var h=this._bounds,u=this._position;this._changed(9);var l=h&&t&&t.decompose();if(l&&!l.shearing&&0===l.rotation%90){for(var c in h){var d=h[c];(i||!d._internal)&&t._transformBounds(d,d)}var f=this._boundsGetter,d=h[f&&f.getBounds||f||"getBounds"];d&&(this._position=d.getCenter(!0)),this._bounds=h}else t&&u&&(this._position=t._transformPoint(u,u));return this},_transformContent:function(t){var e=this._children;if(e){for(var n=0,i=e.length;i>n;n++)e[n].transform(t,!0);return!0}},globalToLocal:function(){return this.getGlobalMatrix(!0)._inverseTransform(h.read(arguments))},localToGlobal:function(){return this.getGlobalMatrix(!0)._transformPoint(h.read(arguments))},parentToLocal:function(){return this._matrix._inverseTransform(h.read(arguments))},localToParent:function(){return this._matrix._transformPoint(h.read(arguments))},fitBounds:function(t,e){t=f.read(arguments);var n=this.getBounds(),i=n.height/n.width,r=t.height/t.width,s=(e?i>r:r>i)?t.width/n.width:t.height/n.height,a=new f(new h,new c(n.width*s,n.height*s));a.setCenter(t.getCenter()),this.setBounds(a)},_setStyles:function(t){var e=this._style,n=e.getFillColor(),i=e.getStrokeColor(),r=e.getShadowColor();if(n&&(t.fillStyle=n.toCanvasStyle(t)),i){var s=e.getStrokeWidth();if(s>0){t.strokeStyle=i.toCanvasStyle(t),t.lineWidth=s;var a=e.getStrokeJoin(),o=e.getStrokeCap(),h=e.getMiterLimit();if(a&&(t.lineJoin=a),o&&(t.lineCap=o),h&&(t.miterLimit=h),paper.support.nativeDash){var u=e.getDashArray(),l=e.getDashOffset();u&&u.length&&("setLineDash"in t?(t.setLineDash(u),t.lineDashOffset=l):(t.mozDash=u,t.mozDashOffset=l))}}}if(r){var c=e.getShadowBlur();if(c>0){t.shadowColor=r.toCanvasStyle(t),t.shadowBlur=c;var d=this.getShadowOffset();t.shadowOffsetX=d.x,t.shadowOffsetY=d.y}}},draw:function(t,e,n){function i(t){return a?a.chain(t):t}var r=this._updateVersion=this._project._updateVersion;if(this._visible&&0!==this._opacity){var s=e.matrices,a=e.viewMatrix,o=this._matrix,h=s[s.length-1].chain(o);if(h.isInvertible()){s.push(h),e.updateMatrix&&(h._updateVersion=r,this._globalMatrix=h);var u,l,c,d=this._blendMode,f=this._opacity,_="normal"===d,g=Q.nativeModes[d],p=_&&1===f||e.dontStart||e.clip||(g||_&&1>f)&&this._canComposite(),v=e.pixelRatio;if(!p){var m=this.getStrokeBounds(i(h));if(!m.width||!m.height)return;c=e.offset,l=e.offset=m.getTopLeft().floor(),u=t,t=K.getContext(m.getSize().ceil().add(1).multiply(v)),1!==v&&t.scale(v,v)}t.save();var y=n?n.chain(o):!this.getStrokeScaling(!0)&&i(h),w=!p&&e.clipItem,x=!y||w;if(p?(t.globalAlpha=f,g&&(t.globalCompositeOperation=d)):x&&t.translate(-l.x,-l.y),x&&(p?o:i(h)).applyToContext(t),w&&e.clipItem.draw(t,e.extend({clip:!0})),y){t.setTransform(v,0,0,v,0,0);var b=e.offset;b&&t.translate(-b.x,-b.y)}this._draw(t,e,y),t.restore(),s.pop(),e.clip&&!e.dontFinish&&t.clip(),p||(Q.process(d,t,u,f,l.subtract(c).multiply(v)),K.release(t),e.offset=c)}}},_isUpdated:function(t){var e=this._parent;if(e instanceof T)return e._isUpdated(t);var n=this._updateVersion===t;return!n&&e&&e._visible&&e._isUpdated(t)&&(this._updateVersion=t,n=!0),n},_drawSelection:function(t,e,n,i,r){if((this._drawSelected||this._boundsSelected)&&this._isUpdated(r)){var s=this.getSelectedColor(!0)||this.getLayer().getSelectedColor(!0),a=e.chain(this.getGlobalMatrix(!0));if(t.strokeStyle=t.fillStyle=s?s.toCanvasStyle(t):"#009dec",this._drawSelected&&this._drawSelected(t,a,i),this._boundsSelected){var o=n/2;coords=a._transformCorners(this.getInternalBounds()),t.beginPath();for(var h=0;8>h;h++)t[0===h?"moveTo":"lineTo"](coords[h],coords[++h]);t.closePath(),t.stroke();for(var h=0;8>h;h++)t.fillRect(coords[h]-o,coords[++h]-o,n,n)}}},_canComposite:function(){return!1}},e.each(["down","drag","up","move"],function(t){this["removeOn"+e.capitalize(t)]=function(){var e={};return e[t]=!0,this.removeOn(e)}},{removeOn:function(t){for(var e in t)if(t[e]){var n="mouse"+e,i=this._project,r=i._removeSets=i._removeSets||{};r[n]=r[n]||{},r[n][this._id]=this}return this}})),w=y.extend({_class:"Group",_selectChildren:!0,_serializeFields:{children:[]},initialize:function(t){this._children=[],this._namedChildren={},this._initialize(t)||this.addChildren(Array.isArray(t)?t:arguments)},_changed:function ce(e){ce.base.call(this,e),1026&e&&(this._clipItem=t)},_getClipItem:function(){var e=this._clipItem;if(e===t){e=null;for(var n=0,i=this._children.length;i>n;n++){var r=this._children[n];if(r._clipMask){e=r;break}}this._clipItem=e}return e},isClipped:function(){return!!this._getClipItem()},setClipped:function(t){var e=this.getFirstChild();e&&e.setClipMask(t)},_draw:function(t,e){var n=e.clip,i=!n&&this._getClipItem(),r=!0;if(e=e.extend({clipItem:i,clip:!1}),n?this._currentPath?(t.currentPath=this._currentPath,r=!1):(t.beginPath(),e.dontStart=e.dontFinish=!0):i&&i.draw(t,e.extend({clip:!0})),r)for(var s=0,a=this._children.length;a>s;s++){var o=this._children[s];o!==i&&o.draw(t,e)}n&&(this._currentPath=t.currentPath)}}),x=w.extend({_class:"Layer",initialize:function(n){var i=e.isPlainObject(n)?new e(n):{children:Array.isArray(n)?n:arguments},r=i.insert;i.insert=!1,w.call(this,i),(r||r===t)&&(this._project.addChild(this),this.activate())},_remove:function de(t){if(this._parent)return de.base.call(this,t);if(null!=this._index){var n=this._project;return n._activeLayer===this&&(n._activeLayer=this.getNextSibling()||this.getPreviousSibling()),e.splice(n.layers,null,this._index,1),this._installEvents(!1),n._needsUpdate=!0,!0}return!1},getNextSibling:function fe(){return this._parent?fe.base.call(this):this._project.layers[this._index+1]||null},getPreviousSibling:function _e(){return this._parent?_e.base.call(this):this._project.layers[this._index-1]||null},isInserted:function ge(){return this._parent?ge.base.call(this):null!=this._index},activate:function(){this._project._activeLayer=this},_insert:function pe(t,n,i){return n instanceof x&&!n._parent?(this._remove(!0,!0),e.splice(n._project.layers,[this],n._index+(t?1:0),0),this._setProject(n._project,!0),this):pe.base.call(this,t,n,i)}}),b=y.extend({_class:"Shape",_applyMatrix:!1,_canApplyMatrix:!1,_boundsSelected:!0,_serializeFields:{type:null,size:null,radius:null},initialize:function(t){this._initialize(t)},_equals:function(t){return this._type===t._type&&this._size.equals(t._size)&&e.equals(this._radius,t._radius)},clone:function(t){var e=new b(y.NO_INSERT);return e.setType(this._type),e.setSize(this._size),e.setRadius(this._radius),this._clone(e,t)},getType:function(){return this._type},setType:function(t){this._type=t},getShape:"#getType",setShape:"#setType",getSize:function(){var t=this._size;return new d(t.width,t.height,this,"setSize")},setSize:function(){var t=c.read(arguments);if(this._size){if(!this._size.equals(t)){var e=this._type,n=t.width,i=t.height;if("rectangle"===e){var r=c.min(this._radius,t.divide(2));this._radius.set(r.width,r.height)}else"circle"===e?(n=i=(n+i)/2,this._radius=n/2):"ellipse"===e&&this._radius.set(n/2,i/2);this._size.set(n,i),this._changed(9)}}else this._size=t.clone()},getRadius:function(){var t=this._radius;return"circle"===this._type?t:new d(t.width,t.height,this,"setRadius")},setRadius:function(t){var e=this._type;if("circle"===e){if(t===this._radius)return;
var n=2*t;this._radius=t,this._size.set(n,n)}else if(t=c.read(arguments),this._radius){if(this._radius.equals(t))return;if(this._radius.set(t.width,t.height),"rectangle"===e){var n=c.max(this._size,t.multiply(2));this._size.set(n.width,n.height)}else"ellipse"===e&&this._size.set(2*t.width,2*t.height)}else this._radius=t.clone();this._changed(9)},isEmpty:function(){return!1},toPath:function(n){var i=new(O[e.capitalize(this._type)])({center:new h,size:this._size,radius:this._radius,insert:!1});return i.setStyle(this._style),i.transform(this._matrix),(n||n===t)&&i.insertAbove(this),i},_draw:function(t,e,n){var i=this._style,r=i.hasFill(),s=i.hasStroke(),a=e.dontFinish||e.clip,o=!n;if(r||s||a){var h=this._type,u=this._radius,l="circle"===h;if(e.dontStart||t.beginPath(),o&&l)t.arc(0,0,u,0,2*Math.PI,!0);else{var c=l?u:u.width,d=l?u:u.height,f=this._size,_=f.width,g=f.height;if(o&&"rect"===h&&0===c&&0===d)t.rect(-_/2,-g/2,_,g);else{var p=_/2,v=g/2,m=.44771525016920644,y=c*m,w=d*m,x=[-p,-v+d,-p,-v+w,-p+y,-v,-p+c,-v,p-c,-v,p-y,-v,p,-v+w,p,-v+d,p,v-d,p,v-w,p-y,v,p-c,v,-p+c,v,-p+y,v,-p,v-w,-p,v-d];n&&n.transform(x,x,32),t.moveTo(x[0],x[1]),t.bezierCurveTo(x[2],x[3],x[4],x[5],x[6],x[7]),p!==c&&t.lineTo(x[8],x[9]),t.bezierCurveTo(x[10],x[11],x[12],x[13],x[14],x[15]),v!==d&&t.lineTo(x[16],x[17]),t.bezierCurveTo(x[18],x[19],x[20],x[21],x[22],x[23]),p!==c&&t.lineTo(x[24],x[25]),t.bezierCurveTo(x[26],x[27],x[28],x[29],x[30],x[31])}}t.closePath()}a||!r&&!s||(this._setStyles(t),r&&(t.fill(i.getWindingRule()),t.shadowColor="rgba(0,0,0,0)"),s&&t.stroke())},_canComposite:function(){return!(this.hasFill()&&this.hasStroke())},_getBounds:function(t,e){var n=new f(this._size).setCenter(0,0);return"getBounds"!==t&&this.hasStroke()&&(n=n.expand(this.getStrokeWidth())),e?e._transformBounds(n):n}},new function(){function t(t,e,n){var i=t._radius;if(!i.isZero())for(var r=t._size.divide(2),s=0;4>s;s++){var a=new h(1&s?1:-1,s>1?1:-1),o=a.multiply(r),u=o.subtract(a.multiply(i)),l=new f(o,u);if((n?l.expand(n):l).contains(e))return u}}function e(t,e){var n=t.getAngleInRadians(),i=2*e.width,r=2*e.height,s=i*Math.sin(n),a=r*Math.cos(n);return i*r/(2*Math.sqrt(s*s+a*a))}return{_contains:function n(e){if("rectangle"===this._type){var i=t(this,e);return i?e.subtract(i).divide(this._radius).getLength()<=1:n.base.call(this,e)}return e.divide(this.size).getLength()<=.5},_hitTestSelf:function i(n,r){var s=!1;if(this.hasStroke()){var a=this._type,o=this._radius,h=this.getStrokeWidth()+2*r.tolerance;if("rectangle"===a){var u=t(this,n,h);if(u){var l=n.subtract(u);s=2*Math.abs(l.getLength()-e(l,o))<=h}else{var c=new f(this._size).setCenter(0,0),d=c.expand(h),_=c.expand(-h);s=d._containsPoint(n)&&!_._containsPoint(n)}}else"ellipse"===a&&(o=e(n,o)),s=2*Math.abs(n.getLength()-o)<=h}return s?new P("stroke",this):i.base.apply(this,arguments)}}},{statics:new function(){function t(t,n,i,r,s){var a=new b(e.getNamed(s));return a._type=t,a._size=i,a._radius=r,a.translate(n)}return{Circle:function(){var n=h.readNamed(arguments,"center"),i=e.readNamed(arguments,"radius");return t("circle",n,new c(2*i),i,arguments)},Rectangle:function(){var e=f.readNamed(arguments,"rectangle"),n=c.min(c.readNamed(arguments,"radius"),e.getSize(!0).divide(2));return t("rectangle",e.getCenter(!0),e.getSize(!0),n,arguments)},Ellipse:function(){var e=b._readEllipse(arguments),n=e.radius;return t("ellipse",e.center,n.multiply(2),n,arguments)},_readEllipse:function(t){var n,i;if(e.hasNamed(t,"radius"))n=h.readNamed(t,"center"),i=c.readNamed(t,"radius");else{var r=f.readNamed(t,"rectangle");n=r.getCenter(!0),i=r.getSize(!0).divide(2)}return{center:n,radius:i}}}}}),C=y.extend({_class:"Raster",_applyMatrix:!1,_canApplyMatrix:!1,_boundsGetter:"getBounds",_boundsSelected:!0,_serializeFields:{source:null},initialize:function(e,n){this._initialize(e,n!==t&&h.read(arguments,1))||("string"==typeof e?this.setSource(e):this.setImage(e)),this._size||(this._size=new c)},_equals:function(t){return this.getSource()===t.getSource()},clone:function(t){var e=new C(y.NO_INSERT),n=this._image,i=this._canvas;if(n)e.setImage(n);else if(i){var r=K.getCanvas(this._size);r.getContext("2d").drawImage(i,0,0),e.setCanvas(r)}return this._clone(e,t)},getSize:function(){var t=this._size;return new d(t.width,t.height,this,"setSize")},setSize:function(){var t=c.read(arguments);if(!this._size.equals(t)){var e=this.getElement();this.setCanvas(K.getCanvas(t)),e&&this.getContext(!0).drawImage(e,0,0,t.width,t.height)}},getWidth:function(){return this._size.width},getHeight:function(){return this._size.height},isEmpty:function(){return 0===this._size.width&&0===this._size.height},getResolution:function(){var t=this._matrix,e=new h(0,0).transform(t),n=new h(1,0).transform(t).subtract(e),i=new h(0,1).transform(t).subtract(e);return new c(72/n.getLength(),72/i.getLength())},getPpi:"#getResolution",getImage:function(){return this._image},setImage:function(t){this._canvas&&K.release(this._canvas),t&&t.getContext?(this._image=null,this._canvas=t):(this._image=t,this._canvas=null),this._size=new c(t?t.naturalWidth||t.width:0,t?t.naturalHeight||t.height:0),this._context=null,this._changed(521)},getCanvas:function(){if(!this._canvas){var t=K.getContext(this._size);try{this._image&&t.drawImage(this._image,0,0),this._canvas=t.canvas}catch(e){K.release(t)}}return this._canvas},setCanvas:"#setImage",getContext:function(t){return this._context||(this._context=this.getCanvas().getContext("2d")),t&&(this._image=null,this._changed(513)),this._context},setContext:function(t){this._context=t},getSource:function(){return this._image&&this._image.src||this.toDataURL()},setSource:function(t){function e(){var t=i.getView();t&&(paper=t._scope,i.setImage(n),i.emit("load"),t.update())}var n,i=this;n=document.getElementById(t)||new Image,n.naturalWidth&&n.naturalHeight?setTimeout(e,0):(V.add(n,{load:e}),n.src||(n.src=t)),this.setImage(n)},getElement:function(){return this._canvas||this._image}},{beans:!1,getSubCanvas:function(){var t=f.read(arguments),e=K.getContext(t.getSize());return e.drawImage(this.getCanvas(),t.x,t.y,t.width,t.height,0,0,t.width,t.height),e.canvas},getSubRaster:function(){var t=f.read(arguments),e=new C(y.NO_INSERT);return e.setCanvas(this.getSubCanvas(t)),e.translate(t.getCenter().subtract(this.getSize().divide(2))),e._matrix.preConcatenate(this._matrix),e.insertAbove(this),e},toDataURL:function(){var t=this._image&&this._image.src;if(/^data:/.test(t))return t;var e=this.getCanvas();return e?e.toDataURL():null},drawImage:function(t){var e=h.read(arguments,1);this.getContext(!0).drawImage(t,e.x,e.y)},getAverageColor:function(t){var n,i;t?t instanceof A?(i=t,n=t.getBounds()):t.width?n=new f(t):t.x&&(n=new f(t.x-.5,t.y-.5,1,1)):n=this.getBounds();var r=32,s=Math.min(n.width,r),a=Math.min(n.height,r),o=C._sampleContext;o?o.clearRect(0,0,r+1,r+1):o=C._sampleContext=K.getContext(new c(r)),o.save();var h=(new g).scale(s/n.width,a/n.height).translate(-n.x,-n.y);h.applyToContext(o),i&&i.draw(o,new e({clip:!0,matrices:[h]})),this._matrix.applyToContext(o),o.drawImage(this.getElement(),-this._size.width/2,-this._size.height/2),o.restore();for(var u=o.getImageData(.5,.5,Math.ceil(s),Math.ceil(a)).data,l=[0,0,0],d=0,_=0,p=u.length;p>_;_+=4){var v=u[_+3];d+=v,v/=255,l[0]+=u[_]*v,l[1]+=u[_+1]*v,l[2]+=u[_+2]*v}for(var _=0;3>_;_++)l[_]/=d;return d?B.read(l):null},getPixel:function(){var t=h.read(arguments),e=this.getContext().getImageData(t.x,t.y,1,1).data;return new B("rgb",[e[0]/255,e[1]/255,e[2]/255],e[3]/255)},setPixel:function(){var t=h.read(arguments),e=B.read(arguments),n=e._convert("rgb"),i=e._alpha,r=this.getContext(!0),s=r.createImageData(1,1),a=s.data;a[0]=255*n[0],a[1]=255*n[1],a[2]=255*n[2],a[3]=null!=i?255*i:255,r.putImageData(s,t.x,t.y)},createImageData:function(){var t=c.read(arguments);return this.getContext().createImageData(t.width,t.height)},getImageData:function(){var t=f.read(arguments);return t.isEmpty()&&(t=new f(this._size)),this.getContext().getImageData(t.x,t.y,t.width,t.height)},setImageData:function(t){var e=h.read(arguments,1);this.getContext(!0).putImageData(t,e.x,e.y)},_getBounds:function(t,e){var n=new f(this._size).setCenter(0,0);return e?e._transformBounds(n):n},_hitTestSelf:function(t){if(this._contains(t)){var e=this;return new P("pixel",e,{offset:t.add(e._size.divide(2)).round(),color:{get:function(){return e.getPixel(this.offset)}}})}},_draw:function(t){var e=this.getElement();e&&(t.globalAlpha=this._opacity,t.drawImage(e,-this._size.width/2,-this._size.height/2))},_canComposite:function(){return!0}}),S=y.extend({_class:"PlacedSymbol",_applyMatrix:!1,_canApplyMatrix:!1,_boundsGetter:{getBounds:"getStrokeBounds"},_boundsSelected:!0,_serializeFields:{symbol:null},initialize:function(e,n){this._initialize(e,n!==t&&h.read(arguments,1))||this.setSymbol(e instanceof m?e:new m(e))},_equals:function(t){return this._symbol===t._symbol},getSymbol:function(){return this._symbol},setSymbol:function(t){this._symbol=t,this._changed(9)},clone:function(t){var e=new S(y.NO_INSERT);return e.setSymbol(this._symbol),this._clone(e,t)},isEmpty:function(){return this._symbol._definition.isEmpty()},_getBounds:function(t,e,n){var i=this.symbol._definition;return i._getCachedBounds(t,e&&e.chain(i._matrix),n)},_hitTestSelf:function(t,e){var n=this._symbol._definition._hitTest(t,e);return n&&(n.item=this),n},_draw:function(t,e){this.symbol._definition.draw(t,e)}}),P=e.extend({_class:"HitResult",initialize:function(t,e,n){this.type=t,this.item=e,n&&(n.enumerable=!0,this.inject(n))},statics:{getOptions:function(t){return new e({type:null,tolerance:paper.settings.hitTolerance,fill:!t,stroke:!t,segments:!t,handles:!1,ends:!1,center:!1,bounds:!1,guides:!1,selected:!1},t)}}}),k=e.extend({_class:"Segment",beans:!0,initialize:function(e,n,i,r,s,a){var o,h,u,l=arguments.length;0===l||(1===l?e.point?(o=e.point,h=e.handleIn,u=e.handleOut):o=e:2===l&&"number"==typeof e?o=arguments:3>=l?(o=e,h=n,u=i):(o=e!==t?[e,n]:null,h=i!==t?[i,r]:null,u=s!==t?[s,a]:null)),new M(o,this,"_point"),new M(h,this,"_handleIn"),new M(u,this,"_handleOut")},_serialize:function(t){return e.serialize(this.isLinear()?this._point:[this._point,this._handleIn,this._handleOut],t,!0)},_changed:function(t){var e=this._path;if(e){var n,i=e._curves,r=this._index;i&&(t&&t!==this._point&&t!==this._handleIn||!(n=r>0?i[r-1]:e._closed?i[i.length-1]:null)||n._changed(),t&&t!==this._point&&t!==this._handleOut||!(n=i[r])||n._changed()),e._changed(25)}},getPoint:function(){return this._point},setPoint:function(){var t=h.read(arguments);this._point.set(t.x,t.y)},getHandleIn:function(){return this._handleIn},setHandleIn:function(){var t=h.read(arguments);this._handleIn.set(t.x,t.y)},getHandleOut:function(){return this._handleOut},setHandleOut:function(){var t=h.read(arguments);this._handleOut.set(t.x,t.y)},isLinear:function(){return this._handleIn.isZero()&&this._handleOut.isZero()},setLinear:function(t){t&&(this._handleIn.set(0,0),this._handleOut.set(0,0))},isColinear:function(t){var e=this.getNext(),n=t.getNext();return this._handleOut.isZero()&&e._handleIn.isZero()&&t._handleOut.isZero()&&n._handleIn.isZero()&&e._point.subtract(this._point).isColinear(n._point.subtract(t._point))},isOrthogonal:function(){var t=this.getPrevious(),e=this.getNext();return t._handleOut.isZero()&&this._handleIn.isZero()&&this._handleOut.isZero()&&e._handleIn.isZero()&&this._point.subtract(t._point).isOrthogonal(e._point.subtract(this._point))},isArc:function(){var t=this.getNext(),e=this._handleOut,n=t._handleIn,i=.5522847498307936;if(e.isOrthogonal(n)){var r=this._point,s=t._point,a=new p(r,e,!0).intersect(new p(s,n,!0),!0);return a&&o.isZero(e.getLength()/a.subtract(r).getLength()-i)&&o.isZero(n.getLength()/a.subtract(s).getLength()-i)}return!1},_selectionState:0,isSelected:function(t){var e=this._selectionState;return t?t===this._point?!!(4&e):t===this._handleIn?!!(1&e):t===this._handleOut?!!(2&e):!1:!!(7&e)},setSelected:function(t,e){var n=this._path,t=!!t,i=this._selectionState,r=i,s=e?e===this._point?4:e===this._handleIn?1:e===this._handleOut?2:0:7;t?i|=s:i&=~s,this._selectionState=i,n&&i!==r&&(n._updateSelection(this,r,i),n._changed(129))},getIndex:function(){return this._index!==t?this._index:null},getPath:function(){return this._path||null},getCurve:function(){var t=this._path,e=this._index;return t?(e>0&&!t._closed&&e===t._segments.length-1&&e--,t.getCurves()[e]||null):null},getLocation:function(){var t=this.getCurve();return t?new z(t,this===t._segment1?0:1):null},getNext:function(){var t=this._path&&this._path._segments;return t&&(t[this._index+1]||this._path._closed&&t[0])||null},getPrevious:function(){var t=this._path&&this._path._segments;return t&&(t[this._index-1]||this._path._closed&&t[t.length-1])||null},reverse:function(){return new k(this._point,this._handleOut,this._handleIn)},remove:function(){return this._path?!!this._path.removeSegment(this._index):!1},clone:function(){return new k(this._point,this._handleIn,this._handleOut)},equals:function(t){return t===this||t&&this._class===t._class&&this._point.equals(t._point)&&this._handleIn.equals(t._handleIn)&&this._handleOut.equals(t._handleOut)||!1},toString:function(){var t=["point: "+this._point];return this._handleIn.isZero()||t.push("handleIn: "+this._handleIn),this._handleOut.isZero()||t.push("handleOut: "+this._handleOut),"{ "+t.join(", ")+" }"},transform:function(t){this._transformCoordinates(t,Array(6),!0),this._changed()},_transformCoordinates:function(t,e,n){var i=this._point,r=n&&this._handleIn.isZero()?null:this._handleIn,s=n&&this._handleOut.isZero()?null:this._handleOut,a=i._x,o=i._y,h=2;return e[0]=a,e[1]=o,r&&(e[h++]=r._x+a,e[h++]=r._y+o),s&&(e[h++]=s._x+a,e[h++]=s._y+o),t&&(t._transformCoordinates(e,e,h/2),a=e[0],o=e[1],n?(i._x=a,i._y=o,h=2,r&&(r._x=e[h++]-a,r._y=e[h++]-o),s&&(s._x=e[h++]-a,s._y=e[h++]-o)):(r||(e[h++]=a,e[h++]=o),s||(e[h++]=a,e[h++]=o))),e}}),M=h.extend({initialize:function(e,n,i){var r,s,a;if(e)if((r=e[0])!==t)s=e[1];else{var o=e;(r=o.x)===t&&(o=h.read(arguments),r=o.x),s=o.y,a=o.selected}else r=s=0;this._x=r,this._y=s,this._owner=n,n[i]=this,a&&this.setSelected(!0)},set:function(t,e){return this._x=t,this._y=e,this._owner._changed(this),this},_serialize:function(t){var e=t.formatter,n=e.number(this._x),i=e.number(this._y);return this.isSelected()?{x:n,y:i,selected:!0}:[n,i]},getX:function(){return this._x},setX:function(t){this._x=t,this._owner._changed(this)},getY:function(){return this._y},setY:function(t){this._y=t,this._owner._changed(this)},isZero:function(){return o.isZero(this._x)&&o.isZero(this._y)},setSelected:function(t){this._owner.setSelected(t,this)},isSelected:function(){return this._owner.isSelected(this)}}),I=e.extend({_class:"Curve",initialize:function(t,e,n,i,r,s,a,o){var h=arguments.length;if(3===h)this._path=t,this._segment1=e,this._segment2=n;else if(0===h)this._segment1=new k,this._segment2=new k;else if(1===h)this._segment1=new k(t.segment1),this._segment2=new k(t.segment2);else if(2===h)this._segment1=new k(t),this._segment2=new k(e);else{var u,l,c,d;4===h?(u=t,l=e,c=n,d=i):8===h&&(u=[t,e],d=[a,o],l=[n-t,i-e],c=[r-a,s-o]),this._segment1=new k(u,null,l),this._segment2=new k(d,c,null)}},_changed:function(){this._length=this._bounds=t},getPoint1:function(){return this._segment1._point},setPoint1:function(){var t=h.read(arguments);this._segment1._point.set(t.x,t.y)},getPoint2:function(){return this._segment2._point},setPoint2:function(){var t=h.read(arguments);this._segment2._point.set(t.x,t.y)},getHandle1:function(){return this._segment1._handleOut},setHandle1:function(){var t=h.read(arguments);this._segment1._handleOut.set(t.x,t.y)},getHandle2:function(){return this._segment2._handleIn},setHandle2:function(){var t=h.read(arguments);this._segment2._handleIn.set(t.x,t.y)},getSegment1:function(){return this._segment1},getSegment2:function(){return this._segment2},getPath:function(){return this._path},getIndex:function(){return this._segment1._index},getNext:function(){var t=this._path&&this._path._curves;return t&&(t[this._segment1._index+1]||this._path._closed&&t[0])||null},getPrevious:function(){var t=this._path&&this._path._curves;return t&&(t[this._segment1._index-1]||this._path._closed&&t[t.length-1])||null},isSelected:function(){return this.getPoint1().isSelected()&&this.getHandle2().isSelected()&&this.getHandle2().isSelected()&&this.getPoint2().isSelected()},setSelected:function(t){this.getPoint1().setSelected(t),this.getHandle1().setSelected(t),this.getHandle2().setSelected(t),this.getPoint2().setSelected(t)},getValues:function(t){return I.getValues(this._segment1,this._segment2,t)},getPoints:function(){for(var t=this.getValues(),e=[],n=0;8>n;n+=2)e.push(new h(t[n],t[n+1]));return e},getLength:function(){return null==this._length&&(this._length=this.isLinear()?this._segment2._point.getDistance(this._segment1._point):I.getLength(this.getValues(),0,1)),this._length},getArea:function(){return I.getArea(this.getValues())},getPart:function(t,e){return new I(I.getPart(this.getValues(),t,e))},getPartLength:function(t,e){return I.getLength(this.getValues(),t,e)},isLinear:function(){return this._segment1._handleOut.isZero()&&this._segment2._handleIn.isZero()},isHorizontal:function(){return this.isLinear()&&o.isZero(this._segment1._point._y-this._segment2._point._y)},getIntersections:function(t){return I.getIntersections(this.getValues(),t.getValues(),this,t,[])},_getParameter:function(e,n){return n?e:e&&e.curve===this?e.parameter:e===t&&n===t?.5:this.getParameterAt(e,0)},divide:function(t,e,n){var i=this._getParameter(t,e),r=1e-5,s=null;if(i>r&&1-r>i){var a=I.subdivide(this.getValues(),i),o=n?!1:this.isLinear(),u=a[0],l=a[1];o||(this._segment1._handleOut.set(u[2]-u[0],u[3]-u[1]),this._segment2._handleIn.set(l[4]-l[6],l[5]-l[7]));var c=u[6],d=u[7],f=new k(new h(c,d),!o&&new h(u[4]-c,u[5]-d),!o&&new h(l[2]-c,l[3]-d));if(this._path)this._segment1._index>0&&0===this._segment2._index?this._path.add(f):this._path.insert(this._segment2._index,f),s=this;else{var _=this._segment2;this._segment2=f,s=new I(f,_)}}return s},split:function(t,e){return this._path?this._path.split(this._segment1._index,this._getParameter(t,e)):null},reverse:function(){return new I(this._segment2.reverse(),this._segment1.reverse())},remove:function(){var t=!1;if(this._path){var e=this._segment2,n=e._handleOut;t=e.remove(),t&&this._segment1._handleOut.set(n.x,n.y)}return t},clone:function(){return new I(this._segment1,this._segment2)},toString:function(){var t=["point1: "+this._segment1._point];return this._segment1._handleOut.isZero()||t.push("handle1: "+this._segment1._handleOut),this._segment2._handleIn.isZero()||t.push("handle2: "+this._segment2._handleIn),t.push("point2: "+this._segment2._point),"{ "+t.join(", ")+" }"},statics:{getValues:function(t,e,n){var i=t._point,r=t._handleOut,s=e._handleIn,a=e._point,o=[i._x,i._y,i._x+r._x,i._y+r._y,a._x+s._x,a._y+s._y,a._x,a._y];return n&&n._transformCoordinates(o,o,4),o},evaluate:function(t,e,n){var i,r,s=t[0],a=t[1],o=t[2],u=t[3],l=t[4],c=t[5],d=t[6],f=t[7],_=1e-5;if(0===n&&(_>e||e>1-_)){var g=_>e;i=g?s:d,r=g?a:f}else{var p=3*(o-s),v=3*(l-o)-p,m=d-s-p-v,y=3*(u-a),w=3*(c-u)-y,x=f-a-y-w;if(0===n)i=((m*e+v)*e+p)*e+s,r=((x*e+w)*e+y)*e+a;else if(_>e&&o===s&&u===a||e>1-_&&l===d&&c===f?(i=l-o,r=c-u):_>e?(i=p,r=y):e>1-_?(i=3*(d-l),r=3*(f-c)):(i=(3*m*e+2*v)*e+p,r=(3*x*e+2*w)*e+y),3===n){var b=6*m*e+2*v,C=6*x*e+2*w;return(i*C-r*b)/Math.pow(i*i+r*r,1.5)}}return 2===n?new h(r,-i):new h(i,r)},subdivide:function(e,n){var i=e[0],r=e[1],s=e[2],a=e[3],o=e[4],h=e[5],u=e[6],l=e[7];n===t&&(n=.5);var c=1-n,d=c*i+n*s,f=c*r+n*a,_=c*s+n*o,g=c*a+n*h,p=c*o+n*u,v=c*h+n*l,m=c*d+n*_,y=c*f+n*g,w=c*_+n*p,x=c*g+n*v,b=c*m+n*w,C=c*y+n*x;return[[i,r,d,f,m,y,b,C],[b,C,w,x,p,v,u,l]]},solveCubic:function(t,e,n,i,r,s){var a=t[e],h=t[e+2],u=t[e+4],l=t[e+6],c=3*(h-a),d=3*(u-h)-c,f=l-a-c-d;return o.solveCubic(f,d,c,a-n,i,r,s)},getParameterOf:function(t,e,n){var i=1e-5;if(Math.abs(t[0]-e)<i&&Math.abs(t[1]-n)<i)return 0;if(Math.abs(t[6]-e)<i&&Math.abs(t[7]-n)<i)return 1;for(var r,s,a=[],o=[],h=I.solveCubic(t,0,e,a,0,1),u=I.solveCubic(t,1,n,o,0,1),l=0;-1==h||h>l;)if(-1==h||(r=a[l++])>=0&&1>=r){for(var c=0;-1==u||u>c;)if((-1==u||(s=o[c++])>=0&&1>=s)&&(-1==h?r=s:-1==u&&(s=r),Math.abs(r-s)<i))return.5*(r+s);if(-1==h)break}return null},getPart:function(t,e,n){return e>0&&(t=I.subdivide(t,e)[1]),1>n&&(t=I.subdivide(t,(n-e)/(1-e))[0]),t},isLinear:function(t){var e=o.isZero;return e(t[0]-t[2])&&e(t[1]-t[3])&&e(t[4]-t[6])&&e(t[5]-t[7])},isFlatEnough:function(t,e){var n=t[0],i=t[1],r=t[2],s=t[3],a=t[4],o=t[5],h=t[6],u=t[7],l=3*r-2*n-h,c=3*s-2*i-u,d=3*a-2*h-n,f=3*o-2*u-i;return Math.max(l*l,d*d)+Math.max(c*c,f*f)<10*e*e},getArea:function(t){var e=t[0],n=t[1],i=t[2],r=t[3],s=t[4],a=t[5],o=t[6],h=t[7];return(3*r*e-1.5*r*s-1.5*r*o-3*n*i-1.5*n*s-.5*n*o+1.5*a*e+1.5*a*i-3*a*o+.5*h*e+1.5*h*i+3*h*s)/10},getBounds:function(t){for(var e=t.slice(0,2),n=e.slice(),i=[0,0],r=0;2>r;r++)I._addBounds(t[r],t[r+2],t[r+4],t[r+6],r,0,e,n,i);return new f(e[0],e[1],n[0]-e[0],n[1]-e[1])},_addBounds:function(t,e,n,i,r,s,a,h,u){function l(t,e){var n=t-e,i=t+e;n<a[r]&&(a[r]=n),i>h[r]&&(h[r]=i)}var c=3*(e-n)-t+i,d=2*(t+n)-4*e,f=e-t,_=o.solveQuadratic(c,d,f,u),g=1e-5,p=1-g;l(i,0);for(var v=0;_>v;v++){var m=u[v],y=1-m;m>g&&p>m&&l(y*y*y*t+3*y*y*m*e+3*y*m*m*n+m*m*m*i,s)}}}},e.each(["getBounds","getStrokeBounds","getHandleBounds","getRoughBounds"],function(t){this[t]=function(){this._bounds||(this._bounds={});var e=this._bounds[t];return e||(e=this._bounds[t]=O[t]([this._segment1,this._segment2],!1,this._path.getStyle())),e.clone()}},{}),e.each(["getPoint","getTangent","getNormal","getCurvature"],function(t,e){this[t+"At"]=function(t,n){var i=this.getValues();return I.evaluate(i,n?t:I.getParameterAt(i,t,0),e)},this[t]=function(t){return I.evaluate(this.getValues(),t,e)}},{beans:!1,getParameterAt:function(t,e){return I.getParameterAt(this.getValues(),t,e)},getParameterOf:function(){var t=h.read(arguments);return I.getParameterOf(this.getValues(),t.x,t.y)},getLocationAt:function(t,e){return e||(t=this.getParameterAt(t)),t>=0&&1>=t&&new z(this,t)},getLocationOf:function(){return this.getLocationAt(this.getParameterOf(h.read(arguments)),!0)},getOffsetOf:function(){var t=this.getLocationOf.apply(this,arguments);return t?t.getOffset():null},getNearestLocation:function(){function t(t){if(t>=0&&1>=t){var i=e.getDistance(I.evaluate(n,t,0),!0);if(r>i)return r=i,s=t,!0}}for(var e=h.read(arguments),n=this.getValues(),i=100,r=1/0,s=0,a=0;i>=a;a++)t(a/i);for(var o=1/(2*i);o>1e-5;)t(s-o)||t(s+o)||(o/=2);var u=I.evaluate(n,s,0);return new z(this,s,u,null,null,null,e.getDistance(u))},getNearestPoint:function(){return this.getNearestLocation.apply(this,arguments).getPoint()}}),new function(){function e(t){var e=t[0],n=t[1],i=t[2],r=t[3],s=t[4],a=t[5],o=t[6],h=t[7],u=9*(i-s)+3*(o-e),l=6*(e+s)-12*i,c=3*(i-e),d=9*(r-a)+3*(h-n),f=6*(n+a)-12*r,_=3*(r-n);return function(t){var e=(u*t+l)*t+c,n=(d*t+f)*t+_;return Math.sqrt(e*e+n*n)}}function n(t,e){return Math.max(2,Math.min(16,Math.ceil(32*Math.abs(e-t))))}return{statics:!0,getLength:function(i,r,s){r===t&&(r=0),s===t&&(s=1);var a=o.isZero;if(0===r&&1===s&&a(i[0]-i[2])&&a(i[1]-i[3])&&a(i[6]-i[4])&&a(i[7]-i[5])){var h=i[6]-i[0],u=i[7]-i[1];return Math.sqrt(h*h+u*u)}var l=e(i);return o.integrate(l,r,s,n(r,s))},getParameterAt:function(i,r,s){function a(t){return _+=o.integrate(c,s,t,n(s,t)),s=t,_-r}if(s===t&&(s=0>r?1:0),0===r)return s;var h=r>0,u=h?s:0,l=h?1:s,c=e(i),d=o.integrate(c,u,l,n(u,l));if(Math.abs(r)>=d)return h?l:u;var f=r/d,_=0;return o.findRoot(a,c,s+f,u,l,16,1e-5)}}},new function(){function t(t,e,n,i,r,s,a,o){var h=new z(n,i,r,s,a,o);(!e||e(h))&&t.push(h)}function e(r,s,a,o,h,u,l,c,d,f,_,g,v){if(!(v>20)){var m,y,w,x=s[0],b=s[1],C=s[6],S=s[7],P=1e-5,k=1e-9,M=p.getSignedDistance,z=M(x,b,C,S,s[2],s[3])||0,A=M(x,b,C,S,s[4],s[5])||0,O=z*A>0?.75:4/9,T=O*Math.min(0,z,A),L=O*Math.max(0,z,A),E=M(x,b,C,S,r[0],r[1]),N=M(x,b,C,S,r[2],r[3]),j=M(x,b,C,S,r[4],r[5]),B=M(x,b,C,S,r[6],r[7]);if(x===C&&k>=f-d&&v>3)m=(c+l)/2,y=m,w=0;else{var D,R,F=n(E,N,j,B),q=F[0],V=F[1];if(D=i(q,V,T,L),q.reverse(),V.reverse(),R=i(q,V,T,L),null==D||null==R)return!1;r=I.getPart(r,D,R),w=R-D,m=c*D+l*(1-D),y=c*R+l*(1-R)}if(_>.8&&w>.8)if(y-m>f-d){var Z=I.subdivide(r,.5),U=m+(y-m)/2;e(s,Z[0],o,a,h,u,d,f,m,U,w,!g,++v),e(s,Z[1],o,a,h,u,d,f,U,y,w,!g,v)}else{var Z=I.subdivide(s,.5),U=d+(f-d)/2;e(Z[0],r,o,a,h,u,d,U,m,y,w,!g,++v),e(Z[1],r,o,a,h,u,U,f,m,y,w,!g,v)}else if(Math.max(f-d,y-m)<P){var H=m+(y-m)/2,W=d+(f-d)/2;g?t(h,u,o,W,I.evaluate(s,W,0),a,H,I.evaluate(r,H,0)):t(h,u,a,H,I.evaluate(r,H,0),o,W,I.evaluate(s,W,0))}else e(s,r,o,a,h,u,d,f,m,y,w,!g,++v)}}function n(t,e,n,i){var r,s=[0,t],a=[1/3,e],o=[2/3,n],h=[1,i],u=p.getSignedDistance,l=u(0,t,1,i,1/3,e),c=u(0,t,1,i,2/3,n),d=!1;if(0>l*c)r=[[s,a,h],[s,o,h]],d=0>l;else{var f,_=0,g=0===l||0===c;Math.abs(l)>Math.abs(c)?(f=a,_=(i-n-(i-t)/3)*(2*(i-n)-i+e)/3):(f=o,_=(e-t+(t-i)/3)*(-2*(t-e)+t-n)/3),r=0>_||g?[[s,f,h],[s,h]]:[[s,a,o,h],[s,h]],d=l?0>l:0>c}return d?r.reverse():r}function i(t,e,n,i){for(var r,s,a,o,h,u=null,l=0,c=e.length-1;c>l;l++){if(a=e[l][1],h=e[l+1][1],h>a)r=null;else{if(!(i>=h))continue;s=e[l][0],o=e[l+1][0],r=s+(i-a)*(o-s)/(h-a)}break}t[0][1]<=i&&(r=t[0][0]);for(var l=0,c=t.length-1;c>l;l++){if(a=t[l][1],h=t[l+1][1],a>=n)u=r;else if(a>h)u=null;else{if(!(h>=n))continue;s=t[l][0],o=t[l+1][0],u=s+(n-a)*(o-s)/(h-a)}break}return u}function r(e,n,i,r,s,a){for(var o=I.isLinear(e),h=o?n:e,u=o?e:n,l=u[0],c=u[1],d=u[6],f=u[7],_=d-l,g=f-c,p=Math.atan2(-g,_),v=Math.sin(p),m=Math.cos(p),y=_*m-g*v,w=[0,0,0,0,y,0,y,0],x=[],b=0;8>b;b+=2){var C=h[b]-l,S=h[b+1]-c;x.push(C*m-S*v,S*m+C*v)}for(var P=[],k=I.solveCubic(x,1,0,P,0,1),b=0;k>b;b++){var M=P[b],C=I.evaluate(x,M,0).x;if(C>=0&&y>=C){var z=I.getParameterOf(w,C,0),A=o?z:M,O=o?M:z;t(s,a,i,A,I.evaluate(e,A,0),r,O,I.evaluate(n,O,0))}}}function s(e,n,i,r,s,a){var o=p.intersect(e[0],e[1],e[6],e[7],n[0],n[1],n[6],n[7]);if(o){var h=o.x,u=o.y;t(s,a,i,I.getParameterOf(e,h,u),o,r,I.getParameterOf(n,h,u),o)}}return{statics:{getIntersections:function(t,n,i,a,o,h){var u=I.isLinear(t),l=I.isLinear(n);return(u&&l?s:u||l?r:e)(t,n,i,a,o,h,0,1,0,1,0,!1,0),o}}}}),z=e.extend({_class:"CurveLocation",beans:!0,initialize:function ve(t,e,n,i,r,s,a){this._id=ve._id=(ve._id||0)+1,this._curve=t,this._segment1=t._segment1,this._segment2=t._segment2,this._parameter=e,this._point=n,this._curve2=i,this._parameter2=r,this._point2=s,this._distance=a},getSegment:function(t){if(!this._segment){var e=this.getCurve(),n=this.getParameter();if(1===n)this._segment=e._segment2;else if(0===n||t)this._segment=e._segment1;else{if(null==n)return null;this._segment=e.getPartLength(0,n)<e.getPartLength(n,1)?e._segment1:e._segment2}}return this._segment},getCurve:function(t){return(!this._curve||t)&&(this._curve=this._segment1.getCurve(),null==this._curve.getParameterOf(this._point)&&(this._curve=this._segment2.getPrevious().getCurve())),this._curve},getIntersection:function(){var t=this._intersection;if(!t&&this._curve2){var e=this._parameter2;this._intersection=t=new z(this._curve2,e,this._point2||this._point,this),t._intersection=this}return t},getPath:function(){var t=this.getCurve();return t&&t._path},getIndex:function(){var t=this.getCurve();return t&&t.getIndex()},getOffset:function(){var t=this.getPath();return t?t._getOffset(this):this.getCurveOffset()},getCurveOffset:function(){var t=this.getCurve(),e=this.getParameter();return null!=e&&t&&t.getPartLength(0,e)},getParameter:function(t){if((null==this._parameter||t)&&this._point){var e=this.getCurve(t);this._parameter=e&&e.getParameterOf(this._point)}return this._parameter},getPoint:function(t){if((!this._point||t)&&null!=this._parameter){var e=this.getCurve(t);this._point=e&&e.getPointAt(this._parameter,!0)}return this._point},getDistance:function(){return this._distance},divide:function(){var t=this.getCurve(!0);return t&&t.divide(this.getParameter(!0),!0)},split:function(){var t=this.getCurve(!0);return t&&t.split(this.getParameter(!0),!0)},equals:function(t){var e=o.isZero;return this===t||t&&this._curve===t._curve&&this._curve2===t._curve2&&e(this._parameter-t._parameter)&&e(this._parameter2-t._parameter2)||!1},toString:function(){var t=[],e=this.getPoint(),n=a.instance;e&&t.push("point: "+e);var i=this.getIndex();null!=i&&t.push("index: "+i);var r=this.getParameter();return null!=r&&t.push("parameter: "+n.number(r)),null!=this._distance&&t.push("distance: "+n.number(this._distance)),"{ "+t.join(", ")+" }"}},e.each(["getTangent","getNormal","getCurvature"],function(t){var e=t+"At";this[t]=function(){var t=this.getParameter(),n=this.getCurve();return null!=t&&n&&n[e](t,!0)}},{})),A=y.extend({_class:"PathItem",initialize:function(){},getIntersections:function(e,n,i){function r(t,e){var n=t.getPath(),i=e.getPath();return n===i?t.getIndex()+t.getParameter()-(e.getIndex()+e.getParameter()):n._id-i._id}this===e&&(e=null);var s=[],a=this.getCurves(),o=e?e.getCurves():a,h=this._matrix.orNullIfIdentity(),u=e?(n||e._matrix).orNullIfIdentity():h,l=a.length,c=e?o.length:l,d=[],f=1e-11,_=1-1e-11;if(e&&!this.getBounds(h).touches(e.getBounds(u)))return[];for(var g=0;c>g;g++)d[g]=o[g].getValues(u);for(var g=0;l>g;g++){var v=a[g],m=e?v.getValues(h):d[g];if(!e){var y=v.getSegment1(),w=v.getSegment2(),x=y._handleOut,b=w._handleIn;if(new p(y._point.subtract(x),x.multiply(2),!0).intersect(new p(w._point.subtract(b),b.multiply(2),!0),!1)){var C=I.subdivide(m);I.getIntersections(C[0],C[1],v,v,s,function(e){return e._parameter<=_?(e._parameter/=2,e._parameter2=.5+e._parameter2/2,!0):t})}}for(var S=e?0:g+1;c>S;S++)I.getIntersections(m,d[S],v,o[S],s,!e&&(S===g+1||S===c-1&&0===g)&&function(t){var e=t._parameter;return e>=f&&_>=e})}for(var P=s.length-1,g=P;g>=0;g--){var k=s[g],M=k._curve.getNext(),z=k._curve2.getNext();M&&k._parameter>=_&&(k._parameter=0,k._curve=M),z&&k._parameter2>=_&&(k._parameter2=0,k._curve2=z)}if(P>0){s.sort(r);for(var g=P;g>=1;g--)s[g].equals(s[0===g?P:g-1])&&(s.splice(g,1),P--)}if(i){for(var g=P;g>=0;g--)s.push(s[g].getIntersection());s.sort(r)}return s},_asPathItem:function(){return this},setPathData:function(t){function e(t,e){var n=+i[t];return o&&(n+=u[e]),n}function n(t){return new h(e(t,"x"),e(t+1,"y"))}var i,r,s,a=t.match(/[mlhvcsqtaz][^mlhvcsqtaz]*/gi),o=!1,u=new h,l=new h;this.clear();for(var d=0,f=a.length;f>d;d++){var _=a[d],g=_[0],p=g.toLowerCase();i=_.match(/[+-]?(?:\d*\.\d+|\d+\.?)(?:[eE][+-]?\d+)?/g);var v=i&&i.length;switch(o=g===p,"z"!==r||/[mz]/.test(p)||this.moveTo(u=l),p){case"m":case"l":var m="m"===p;m&&r&&"z"!==r&&this.closePath(!0);for(var y=0;v>y;y+=2)this[0===y&&m?"moveTo":"lineTo"](u=n(y));s=u,m&&(l=u);break;case"h":case"v":for(var w="h"===p?"x":"y",y=0;v>y;y++)u[w]=e(y,w),this.lineTo(u);s=u;break;case"c":for(var y=0;v>y;y+=6)this.cubicCurveTo(n(y),s=n(y+2),u=n(y+4));break;case"s":for(var y=0;v>y;y+=4)this.cubicCurveTo(/[cs]/.test(r)?u.multiply(2).subtract(s):u,s=n(y),u=n(y+2)),r=p;break;case"q":for(var y=0;v>y;y+=4)this.quadraticCurveTo(s=n(y),u=n(y+2));break;case"t":for(var y=0;v>y;y+=2)this.quadraticCurveTo(s=/[qt]/.test(r)?u.multiply(2).subtract(s):u,u=n(y)),r=p;break;case"a":for(var y=0;v>y;y+=7)this.arcTo(u=n(y+5),new c(+i[0],+i[1]),+i[2],+i[4],+i[3]);break;case"z":this.closePath(!0)}r=p}},_canComposite:function(){return!(this.hasFill()&&this.hasStroke())},_contains:function(t){var e=this._getWinding(t,!1,!0);return!!("evenodd"===this.getWindingRule()?1&e:e)}}),O=A.extend({_class:"Path",_serializeFields:{segments:[],closed:!1},initialize:function(e){this._closed=!1,this._segments=[];var n=Array.isArray(e)?"object"==typeof e[0]?e:arguments:!e||e.size!==t||e.x===t&&e.point===t?null:arguments;n&&n.length>0?this.setSegments(n):(this._curves=t,this._selectedSegmentState=0,n||"string"!=typeof e||(this.setPathData(e),e=null)),this._initialize(!n&&e)},_equals:function(t){return e.equals(this._segments,t._segments)},clone:function(e){var n=new O(y.NO_INSERT);return n.setSegments(this._segments),n._closed=this._closed,this._clockwise!==t&&(n._clockwise=this._clockwise),this._clone(n,e)
},_changed:function me(e){if(me.base.call(this,e),8&e){var n=this._parent;if(n&&(n._currentPath=t),this._length=this._clockwise=t,this._curves&&!(16&e))for(var i=0,r=this._curves.length;r>i;i++)this._curves[i]._changed();this._monoCurves=t}else 32&e&&(this._bounds=t)},getStyle:function(){var t=this._parent;return(t instanceof T?t:this)._style},getSegments:function(){return this._segments},setSegments:function(e){var n=this.isFullySelected();this._segments.length=0,this._selectedSegmentState=0,this._curves=t,e&&e.length>0&&this._add(k.readAll(e)),n&&this.setFullySelected(!0)},getFirstSegment:function(){return this._segments[0]},getLastSegment:function(){return this._segments[this._segments.length-1]},getCurves:function(){var t=this._curves,e=this._segments;if(!t){var n=this._countCurves();t=this._curves=Array(n);for(var i=0;n>i;i++)t[i]=new I(this,e[i],e[i+1]||e[0])}return t},getFirstCurve:function(){return this.getCurves()[0]},getLastCurve:function(){var t=this.getCurves();return t[t.length-1]},isClosed:function(){return this._closed},setClosed:function(t){if(this._closed!=(t=!!t)){if(this._closed=t,this._curves){var e=this._curves.length=this._countCurves();t&&(this._curves[e-1]=new I(this,this._segments[e-1],this._segments[0]))}this._changed(25)}}},{beans:!0,getPathData:function(t,e){function n(e,n){e._transformCoordinates(t,g,!1),i=g[0],r=g[1],p?(v.push("M"+_.pair(i,r)),p=!1):(h=g[2],u=g[3],h===i&&u===r&&l===s&&c===o?n||v.push("l"+_.pair(i-s,r-o)):v.push("c"+_.pair(l-s,c-o)+" "+_.pair(h-s,u-o)+" "+_.pair(i-s,r-o))),s=i,o=r,l=g[4],c=g[5]}var i,r,s,o,h,u,l,c,d=this._segments,f=d.length,_=new a(e),g=Array(6),p=!0,v=[];if(0===f)return"";for(var m=0;f>m;m++)n(d[m]);return this._closed&&f>0&&(n(d[0],!0),v.push("z")),v.join("")}},{isEmpty:function(){return 0===this._segments.length},isPolygon:function(){for(var t=0,e=this._segments.length;e>t;t++)if(!this._segments[t].isLinear())return!1;return!0},_transformContent:function(t){for(var e=Array(6),n=0,i=this._segments.length;i>n;n++)this._segments[n]._transformCoordinates(t,e,!0);return!0},_add:function(t,e){for(var n=this._segments,i=this._curves,r=t.length,s=null==e,e=s?n.length:e,a=0;r>a;a++){var o=t[a];o._path&&(o=t[a]=o.clone()),o._path=this,o._index=e+a,o._selectionState&&this._updateSelection(o,0,o._selectionState)}if(s)n.push.apply(n,t);else{n.splice.apply(n,[e,0].concat(t));for(var a=e+r,h=n.length;h>a;a++)n[a]._index=a}if(i||t._curves){i||(i=this._curves=[]);var u=e>0?e-1:e,l=u,c=Math.min(u+r,this._countCurves());t._curves&&(i.splice.apply(i,[u,0].concat(t._curves)),l+=t._curves.length);for(var a=l;c>a;a++)i.splice(a,0,new I(this,null,null));this._adjustCurves(u,c)}return this._changed(25),t},_adjustCurves:function(t,e){for(var n,i=this._segments,r=this._curves,s=t;e>s;s++)n=r[s],n._path=this,n._segment1=i[s],n._segment2=i[s+1]||i[0],n._changed();(n=r[this._closed&&0===t?i.length-1:t-1])&&(n._segment2=i[t]||i[0],n._changed()),(n=r[e])&&(n._segment1=i[e],n._changed())},_countCurves:function(){var t=this._segments.length;return!this._closed&&t>0?t-1:t},add:function(t){return arguments.length>1&&"number"!=typeof t?this._add(k.readAll(arguments)):this._add([k.read(arguments)])[0]},insert:function(t,e){return arguments.length>2&&"number"!=typeof e?this._add(k.readAll(arguments,1),t):this._add([k.read(arguments,1)],t)[0]},addSegment:function(){return this._add([k.read(arguments)])[0]},insertSegment:function(t){return this._add([k.read(arguments,1)],t)[0]},addSegments:function(t){return this._add(k.readAll(t))},insertSegments:function(t,e){return this._add(k.readAll(e),t)},removeSegment:function(t){return this.removeSegments(t,t+1)[0]||null},removeSegments:function(t,n,i){t=t||0,n=e.pick(n,this._segments.length);var r=this._segments,s=this._curves,a=r.length,o=r.splice(t,n-t),h=o.length;if(!h)return o;for(var u=0;h>u;u++){var l=o[u];l._selectionState&&this._updateSelection(l,l._selectionState,0),l._index=l._path=null}for(var u=t,c=r.length;c>u;u++)r[u]._index=u;if(s){var d=t>0&&n===a+(this._closed?1:0)?t-1:t,s=s.splice(d,h);i&&(o._curves=s.slice(1)),this._adjustCurves(d,d)}return this._changed(25),o},clear:"#removeSegments",getLength:function(){if(null==this._length){var t=this.getCurves();this._length=0;for(var e=0,n=t.length;n>e;e++)this._length+=t[e].getLength()}return this._length},getArea:function(){for(var t=this.getCurves(),e=0,n=0,i=t.length;i>n;n++)e+=t[n].getArea();return e},isFullySelected:function(){var t=this._segments.length;return this._selected&&t>0&&this._selectedSegmentState===7*t},setFullySelected:function(t){t&&this._selectSegments(!0),this.setSelected(t)},setSelected:function ye(t){t||this._selectSegments(!1),ye.base.call(this,t)},_selectSegments:function(t){var e=this._segments.length;this._selectedSegmentState=t?7*e:0;for(var n=0;e>n;n++)this._segments[n]._selectionState=t?7:0},_updateSelection:function(t,e,n){t._selectionState=n;var i=this._selectedSegmentState+=n-e;i>0&&this.setSelected(!0)},flatten:function(t){for(var e=new L(this,64,.1),n=0,i=e.length/Math.ceil(e.length/t),r=e.length+(this._closed?-i:i)/2,s=[];r>=n;)s.push(new k(e.evaluate(n,0))),n+=i;this.setSegments(s)},reduce:function(){for(var t=this.getCurves(),e=t.length-1;e>=0;e--){var n=t[e];n.isLinear()&&0===n.getLength()&&n.remove()}return this},simplify:function(t){if(this._segments.length>2){var e=new E(this,t||2.5);this.setSegments(e.fit())}},split:function(t,e){if(null!==e){if(1===arguments.length){var n=t;"number"==typeof n&&(n=this.getLocationAt(n)),t=n.index,e=n.parameter}var i=1e-5;e>=1-i&&(t++,e--);var r=this.getCurves();if(t>=0&&t<r.length){e>i&&r[t++].divide(e,!0);var s,a=this.removeSegments(t,this._segments.length,!0);return this._closed?(this.setClosed(!1),s=this):t>0&&(s=this._clone((new O).insertAbove(this,!0))),s._add(a,0),this.addSegment(a[0]),s}return null}},isClockwise:function(){return this._clockwise!==t?this._clockwise:O.isClockwise(this._segments)},setClockwise:function(t){this.isClockwise()!=(t=!!t)&&this.reverse(),this._clockwise=t},reverse:function(){this._segments.reverse();for(var e=0,n=this._segments.length;n>e;e++){var i=this._segments[e],r=i._handleIn;i._handleIn=i._handleOut,i._handleOut=r,i._index=e}this._curves=null,this._clockwise!==t&&(this._clockwise=!this._clockwise),this._changed(9)},join:function(t){if(t){var e=t._segments,n=this.getLastSegment(),i=t.getLastSegment();n._point.equals(i._point)&&t.reverse();var r,s=t.getFirstSegment();n._point.equals(s._point)?(n.setHandleOut(s._handleOut),this._add(e.slice(1))):(r=this.getFirstSegment(),r._point.equals(s._point)&&t.reverse(),i=t.getLastSegment(),r._point.equals(i._point)?(r.setHandleIn(i._handleIn),this._add(e.slice(0,e.length-1),0)):this._add(e.slice())),t.closed&&this._add([e[0]]),t.remove()}var a=this.getFirstSegment(),o=this.getLastSegment();a!==o&&a._point.equals(o._point)&&(a.setHandleIn(o._handleIn),o.remove(),this.setClosed(!0))},toShape:function(e){function n(t,e){return d[t].isColinear(d[e])}function i(t){return d[t].isOrthogonal()}function r(t){return d[t].isArc()}function s(t,e){return d[t]._point.getDistance(d[e]._point)}if(!this._closed)return null;var a,h,u,l,d=this._segments;if(this.isPolygon()&&4===d.length&&n(0,2)&&n(1,3)&&i(1)?(a=b.Rectangle,h=new c(s(0,3),s(0,1)),l=d[1]._point.add(d[2]._point).divide(2)):8===d.length&&r(0)&&r(2)&&r(4)&&r(6)&&n(1,5)&&n(3,7)?(a=b.Rectangle,h=new c(s(1,6),s(0,3)),u=h.subtract(new c(s(0,7),s(1,2))).divide(2),l=d[3]._point.add(d[4]._point).divide(2)):4===d.length&&r(0)&&r(1)&&r(2)&&r(3)&&(o.isZero(s(0,2)-s(1,3))?(a=b.Circle,u=s(0,2)/2):(a=b.Ellipse,u=new c(s(2,0)/2,s(3,1)/2)),l=d[1]._point),a){var f=this.getPosition(!0),_=new a({center:f,size:h,radius:u,insert:!1});return _.rotate(l.subtract(f).getAngle()+90),_.setStyle(this._style),(e||e===t)&&_.insertAbove(this),_}return null},_hitTestSelf:function(t,e){function n(e,n){return t.subtract(e).divide(n).length<=1}function i(t,i,r){if(!e.selected||i.isSelected()){var s=t._point;if(i!==s&&(i=i.add(s)),n(i,w))return new P(r,_,{segment:t,point:i})}}function r(t,n){return(n||e.segments)&&i(t,t._point,"segment")||!n&&e.handles&&(i(t,t._handleIn,"handle-in")||i(t,t._handleOut,"handle-out"))}function s(t){c.add(t)}function a(e){if(("round"!==o||"round"!==u)&&(c=new O({internal:!0,closed:!0}),m||e._index>0&&e._index<v-1?"round"!==o&&(e._handleIn.isZero()||e._handleOut.isZero())&&O._addBevelJoin(e,o,S,l,s,!0):"round"!==u&&O._addSquareCap(e,u,S,s,!0),!c.isEmpty())){var i;return c.contains(t)||(i=c.getNearestLocation(t))&&n(i.getPoint(),y)}return n(e._point,w)}var o,u,l,c,d,f,_=this,g=this.getStyle(),p=this._segments,v=p.length,m=this._closed,y=e._tolerancePadding,w=y,x=e.stroke&&g.hasStroke(),b=e.fill&&g.hasFill(),C=e.curves,S=x?g.getStrokeWidth()/2:b&&e.tolerance>0||C?0:null;if(null!==S&&(S>0?(o=g.getStrokeJoin(),u=g.getStrokeCap(),l=S*g.getMiterLimit(),w=y.add(new h(S,S))):o=u="round"),!e.ends||e.segments||m){if(e.segments||e.handles)for(var k=0;v>k;k++)if(f=r(p[k]))return f}else if(f=r(p[0],!0)||r(p[v-1],!0))return f;if(null!==S){if(d=this.getNearestLocation(t)){var M=d.getParameter();0===M||1===M&&v>1?a(d.getSegment())||(d=null):n(d.getPoint(),w)||(d=null)}if(!d&&"miter"===o&&v>1)for(var k=0;v>k;k++){var I=p[k];if(t.getDistance(I._point)<=l&&a(I)){d=I.getLocation();break}}}return!d&&b&&this._contains(t)||d&&!x&&!C?new P("fill",this):d?new P(x?"stroke":"curve",this,{location:d,point:d.getPoint()}):null}},e.each(["getPoint","getTangent","getNormal","getCurvature"],function(t){this[t+"At"]=function(e,n){var i=this.getLocationAt(e,n);return i&&i[t]()}},{beans:!1,_getOffset:function(t){var e=t&&t.getIndex();if(null!=e){for(var n=this.getCurves(),i=0,r=0;e>r;r++)i+=n[r].getLength();var s=n[e],a=t.getParameter();return a>0&&(i+=s.getPartLength(0,a)),i}return null},getLocationOf:function(){for(var t=h.read(arguments),e=this.getCurves(),n=0,i=e.length;i>n;n++){var r=e[n].getLocationOf(t);if(r)return r}return null},getOffsetOf:function(){var t=this.getLocationOf.apply(this,arguments);return t?t.getOffset():null},getLocationAt:function(t,e){var n=this.getCurves(),i=0;if(e){var r=~~t;return n[r].getLocationAt(t-r,!0)}for(var s=0,a=n.length;a>s;s++){var o=i,h=n[s];if(i+=h.getLength(),i>t)return h.getLocationAt(t-o)}return t<=this.getLength()?new z(n[n.length-1],1):null},getNearestLocation:function(){for(var t=h.read(arguments),e=this.getCurves(),n=1/0,i=null,r=0,s=e.length;s>r;r++){var a=e[r].getNearestLocation(t);a._distance<n&&(n=a._distance,i=a)}return i},getNearestPoint:function(){return this.getNearestLocation.apply(this,arguments).getPoint()}}),new function(){function t(t,e,n,i){function r(e){var n=a[e],i=a[e+1];(c!=n||d!=i)&&(t.beginPath(),t.moveTo(c,d),t.lineTo(n,i),t.stroke(),t.beginPath(),t.arc(n,i,s,0,2*Math.PI,!0),t.fill())}for(var s=i/2,a=Array(6),o=0,h=e.length;h>o;o++){var u=e[o];u._transformCoordinates(n,a,!1);var l=u._selectionState,c=a[0],d=a[1];if(1&l&&r(2),2&l&&r(4),t.fillRect(c-s,d-s,i,i),!(4&l)){var f=t.fillStyle;t.fillStyle="#ffffff",t.fillRect(c-s+1,d-s+1,i-2,i-2),t.fillStyle=f}}}function e(t,e,n){function i(e){if(n)e._transformCoordinates(n,_,!1),r=_[0],s=_[1];else{var i=e._point;r=i._x,s=i._y}if(g)t.moveTo(r,s),g=!1;else{if(n)h=_[2],u=_[3];else{var d=e._handleIn;h=r+d._x,u=s+d._y}h===r&&u===s&&l===a&&c===o?t.lineTo(r,s):t.bezierCurveTo(l,c,h,u,r,s)}if(a=r,o=s,n)l=_[4],c=_[5];else{var d=e._handleOut;l=a+d._x,c=o+d._y}}for(var r,s,a,o,h,u,l,c,d=e._segments,f=d.length,_=Array(6),g=!0,p=0;f>p;p++)i(d[p]);e._closed&&f>0&&i(d[0])}return{_draw:function(t,n,i){function r(t){return l[(t%c+c)%c]}var s=n.dontStart,a=n.dontFinish||n.clip,o=this.getStyle(),h=o.hasFill(),u=o.hasStroke(),l=o.getDashArray(),c=!paper.support.nativeDash&&u&&l&&l.length;if(s||t.beginPath(),!s&&this._currentPath?t.currentPath=this._currentPath:(h||u&&!c||a)&&(e(t,this,i),this._closed&&t.closePath(),s||(this._currentPath=t.currentPath)),!a&&(h||u)&&(this._setStyles(t),h&&(t.fill(o.getWindingRule()),t.shadowColor="rgba(0,0,0,0)"),u)){if(c){s||t.beginPath();var d,f=new L(this,32,.25,i),_=f.length,g=-o.getDashOffset(),p=0;for(g%=_;g>0;)g-=r(p--)+r(p--);for(;_>g;)d=g+r(p++),(g>0||d>0)&&f.drawPart(t,Math.max(g,0),Math.max(d,0)),g=d+r(p++)}t.stroke()}},_drawSelected:function(n,i){n.beginPath(),e(n,this,i),n.stroke(),t(n,this._segments,i,paper.settings.handleSize)}}},new function(){function t(t){var e=t.length,n=[],i=[],r=2;n[0]=t[0]/r;for(var s=1;e>s;s++)i[s]=1/r,r=(e-1>s?4:2)-i[s],n[s]=(t[s]-n[s-1])/r;for(var s=1;e>s;s++)n[e-s-1]-=i[e-s]*n[e-s];return n}return{smooth:function(){var e=this._segments,n=e.length,i=this._closed,r=n,s=0;if(!(2>=n)){i&&(s=Math.min(n,4),r+=2*Math.min(n,s));for(var a=[],o=0;n>o;o++)a[o+s]=e[o]._point;if(i)for(var o=0;s>o;o++)a[o]=e[o+n-s]._point,a[o+n+s]=e[o]._point;else r--;for(var u=[],o=1;r-1>o;o++)u[o]=4*a[o]._x+2*a[o+1]._x;u[0]=a[0]._x+2*a[1]._x,u[r-1]=3*a[r-1]._x;for(var l=t(u),o=1;r-1>o;o++)u[o]=4*a[o]._y+2*a[o+1]._y;u[0]=a[0]._y+2*a[1]._y,u[r-1]=3*a[r-1]._y;var c=t(u);if(i){for(var o=0,d=n;s>o;o++,d++){var f=o/s,_=1-f,g=o+s,p=d+s;l[d]=l[o]*f+l[d]*_,c[d]=c[o]*f+c[d]*_,l[p]=l[g]*_+l[p]*f,c[p]=c[g]*_+c[p]*f}r--}for(var v=null,o=s;r-s>=o;o++){var m=e[o-s];v&&m.setHandleIn(v.subtract(m._point)),r>o&&(m.setHandleOut(new h(l[o],c[o]).subtract(m._point)),v=r-1>o?new h(2*a[o+1]._x-l[o+1],2*a[o+1]._y-c[o+1]):new h((a[r]._x+l[r-1])/2,(a[r]._y+c[r-1])/2))}if(i&&v){var m=this._segments[0];m.setHandleIn(v.subtract(m._point))}}}}},new function(){function t(t){var e=t._segments;if(0===e.length)throw Error("Use a moveTo() command first");return e[e.length-1]}return{moveTo:function(){var t=this._segments;1===t.length&&this.removeSegment(0),t.length||this._add([new k(h.read(arguments))])},moveBy:function(){throw Error("moveBy() is unsupported on Path items.")},lineTo:function(){this._add([new k(h.read(arguments))])},cubicCurveTo:function(){var e=h.read(arguments),n=h.read(arguments),i=h.read(arguments),r=t(this);r.setHandleOut(e.subtract(r._point)),this._add([new k(i,n.subtract(i))])},quadraticCurveTo:function(){var e=h.read(arguments),n=h.read(arguments),i=t(this)._point;this.cubicCurveTo(e.add(i.subtract(e).multiply(1/3)),e.add(n.subtract(e).multiply(1/3)),n)},curveTo:function(){var n=h.read(arguments),i=h.read(arguments),r=e.pick(e.read(arguments),.5),s=1-r,a=t(this)._point,o=n.subtract(a.multiply(s*s)).subtract(i.multiply(r*r)).divide(2*r*s);if(o.isNaN())throw Error("Cannot put a curve through points with parameter = "+r);this.quadraticCurveTo(o,i)},arcTo:function(){var n,i,r,s,a,o=t(this),u=o._point,l=h.read(arguments),d=e.peek(arguments),f=e.pick(d,!0);if("boolean"==typeof f)var _=u.add(l).divide(2),n=_.add(_.subtract(u).rotate(f?-90:90));else if(e.remain(arguments)<=2)n=l,l=h.read(arguments);else{var v=c.read(arguments);if(v.isZero())return this.lineTo(l);var m=e.read(arguments),f=!!e.read(arguments),y=!!e.read(arguments),_=u.add(l).divide(2),w=u.subtract(_).rotate(-m),x=w.x,b=w.y,C=Math.abs,S=1e-11,P=C(v.width),M=C(v.height),I=P*P,z=M*M,A=x*x,O=b*b,T=Math.sqrt(A/I+O/z);if(T>1&&(P*=T,M*=T,I=P*P,z=M*M),T=(I*z-I*O-z*A)/(I*O+z*A),C(T)<S&&(T=0),0>T)throw Error("Cannot create an arc with the given arguments");i=new h(P*b/M,-M*x/P).multiply((y===f?-1:1)*Math.sqrt(T)).rotate(m).add(_),a=(new g).translate(i).rotate(m).scale(P,M),s=a._inverseTransform(u),r=s.getDirectedAngle(a._inverseTransform(l)),!f&&r>0?r-=360:f&&0>r&&(r+=360)}if(n){var L=new p(u.add(n).divide(2),n.subtract(u).rotate(90),!0),E=new p(n.add(l).divide(2),l.subtract(n).rotate(90),!0),N=new p(u,l),j=N.getSide(n);if(i=L.intersect(E,!0),!i){if(!j)return this.lineTo(l);throw Error("Cannot create an arc with the given arguments")}s=u.subtract(i),r=s.getDirectedAngle(l.subtract(i));var B=N.getSide(i);0===B?r=j*Math.abs(r):j===B&&(r+=0>r?360:-360)}for(var D=Math.abs(r),R=D>=360?4:Math.ceil(D/90),F=r/R,q=F*Math.PI/360,V=4/3*Math.sin(q)/(1+Math.cos(q)),Z=[],U=0;R>=U;U++){var w=l,H=null;if(R>U&&(H=s.rotate(90).multiply(V),a?(w=a._transformPoint(s),H=a._transformPoint(s.add(H)).subtract(w)):w=i.add(s)),0===U)o.setHandleOut(H);else{var W=s.rotate(-90).multiply(V);a&&(W=a._transformPoint(s.add(W)).subtract(w)),Z.push(new k(w,W,H))}s=s.rotate(F)}this._add(Z)},lineBy:function(){var e=h.read(arguments),n=t(this)._point;this.lineTo(n.add(e))},curveBy:function(){var n=h.read(arguments),i=h.read(arguments),r=e.read(arguments),s=t(this)._point;this.curveTo(s.add(n),s.add(i),r)},cubicCurveBy:function(){var e=h.read(arguments),n=h.read(arguments),i=h.read(arguments),r=t(this)._point;this.cubicCurveTo(r.add(e),r.add(n),r.add(i))},quadraticCurveBy:function(){var e=h.read(arguments),n=h.read(arguments),i=t(this)._point;this.quadraticCurveTo(i.add(e),i.add(n))},arcBy:function(){var n=t(this)._point,i=n.add(h.read(arguments)),r=e.pick(e.peek(arguments),!0);"boolean"==typeof r?this.arcTo(i,r):this.arcTo(i,n.add(h.read(arguments)))},closePath:function(t){this.setClosed(!0),t&&this.join()}}},{_getBounds:function(t,e){return O[t](this._segments,this._closed,this.getStyle(),e)},statics:{isClockwise:function(t){for(var e=0,n=0,i=t.length;i>n;n++)for(var r=I.getValues(t[n],t[i>n+1?n+1:0]),s=2;8>s;s+=2)e+=(r[s-2]-r[s])*(r[s+1]+r[s-1]);return e>0},getBounds:function(t,e,n,i,r){function s(t){t._transformCoordinates(i,o,!1);for(var e=0;2>e;e++)I._addBounds(h[e],h[e+4],o[e+2],o[e],e,r?r[e]:0,u,l,c);var n=h;h=o,o=n}var a=t[0];if(!a)return new f;for(var o=Array(6),h=a._transformCoordinates(i,Array(6),!1),u=h.slice(0,2),l=u.slice(),c=Array(2),d=1,_=t.length;_>d;d++)s(t[d]);return e&&s(a),new f(u[0],u[1],l[0]-u[0],l[1]-u[1])},getStrokeBounds:function(t,e,n,i){function r(t){d=d.include(i?i._transformPoint(t,t):t)}function s(t){d=d.unite(v.setCenter(i?i._transformPoint(t._point):t._point))}function a(t,e){var n=t._handleIn,i=t._handleOut;"round"===e||!n.isZero()&&!i.isZero()&&n.isColinear(i)?s(t):O._addBevelJoin(t,e,u,p,r)}function o(t,e){"round"===e?s(t):O._addSquareCap(t,e,u,r)}if(!n.hasStroke())return O.getBounds(t,e,n,i);for(var h=t.length-(e?0:1),u=n.getStrokeWidth()/2,l=O._getPenPadding(u,i),d=O.getBounds(t,e,n,i,l),_=n.getStrokeJoin(),g=n.getStrokeCap(),p=u*n.getMiterLimit(),v=new f(new c(l).multiply(2)),m=1;h>m;m++)a(t[m],_);return e?a(t[0],_):h>0&&(o(t[0],g),o(t[t.length-1],g)),d},_getPenPadding:function(t,e){if(!e)return[t,t];var n=e.shiftless(),i=n.transform(new h(t,0)),r=n.transform(new h(0,t)),s=i.getAngleInRadians(),a=i.getLength(),o=r.getLength(),u=Math.sin(s),l=Math.cos(s),c=Math.tan(s),d=-Math.atan(o*c/a),f=Math.atan(o/(c*a));return[Math.abs(a*Math.cos(d)*l-o*Math.sin(d)*u),Math.abs(o*Math.sin(f)*l+a*Math.cos(f)*u)]},_addBevelJoin:function(t,e,n,i,r,s){var a=t.getCurve(),o=a.getPrevious(),u=a.getPointAt(0,!0),l=o.getNormalAt(1,!0),c=a.getNormalAt(0,!0),d=l.getDirectedAngle(c)<0?-n:n;if(l.setLength(d),c.setLength(d),s&&(r(u),r(u.add(l))),"miter"===e){var f=new p(u.add(l),new h(-l.y,l.x),!0).intersect(new p(u.add(c),new h(-c.y,c.x),!0),!0);if(f&&u.getDistance(f)<=i&&(r(f),!s))return}s||r(u.add(l)),r(u.add(c))},_addSquareCap:function(t,e,n,i,r){var s=t._point,a=t.getLocation(),o=a.getNormal().normalize(n);r&&(i(s.subtract(o)),i(s.add(o))),"square"===e&&(s=s.add(o.rotate(0===a.getParameter()?-90:90))),i(s.add(o)),i(s.subtract(o))},getHandleBounds:function(t,e,n,i,r,s){for(var a=Array(6),o=1/0,h=-o,u=o,l=h,c=0,d=t.length;d>c;c++){var _=t[c];_._transformCoordinates(i,a,!1);for(var g=0;6>g;g+=2){var p=0===g?s:r,v=p?p[0]:0,m=p?p[1]:0,y=a[g],w=a[g+1],x=y-v,b=y+v,C=w-m,S=w+m;o>x&&(o=x),b>h&&(h=b),u>C&&(u=C),S>l&&(l=S)}}return new f(o,u,h-o,l-u)},getRoughBounds:function(t,e,n,i){var r=n.hasStroke()?n.getStrokeWidth()/2:0,s=r;return r>0&&("miter"===n.getStrokeJoin()&&(s=r*n.getMiterLimit()),"square"===n.getStrokeCap()&&(s=Math.max(s,r*Math.sqrt(2)))),O.getHandleBounds(t,e,n,i,O._getPenPadding(r,i),O._getPenPadding(s,i))}}});O.inject({statics:new function(){function t(t,n,i){var r=e.getNamed(i),s=new O(r&&r.insert===!1&&y.NO_INSERT);return s._add(t),s._closed=n,s.set(r)}function n(e,n,i){for(var s=Array(4),a=0;4>a;a++){var o=r[a];s[a]=new k(o._point.multiply(n).add(e),o._handleIn.multiply(n),o._handleOut.multiply(n))}return t(s,!0,i)}var i=.5522847498307936,r=[new k([-1,0],[0,i],[0,-i]),new k([0,-1],[-i,0],[i,0]),new k([1,0],[0,-i],[0,i]),new k([0,1],[i,0],[-i,0])];return{Line:function(){return t([new k(h.readNamed(arguments,"from")),new k(h.readNamed(arguments,"to"))],!1,arguments)},Circle:function(){var t=h.readNamed(arguments,"center"),i=e.readNamed(arguments,"radius");return n(t,new c(i),arguments)},Rectangle:function(){var e,n=f.readNamed(arguments,"rectangle"),r=c.readNamed(arguments,"radius",0,{readNull:!0}),s=n.getBottomLeft(!0),a=n.getTopLeft(!0),o=n.getTopRight(!0),h=n.getBottomRight(!0);if(!r||r.isZero())e=[new k(s),new k(a),new k(o),new k(h)];else{r=c.min(r,n.getSize(!0).divide(2));var u=r.width,l=r.height,d=u*i,_=l*i;e=[new k(s.add(u,0),null,[-d,0]),new k(s.subtract(0,l),[0,_]),new k(a.add(0,l),null,[0,-_]),new k(a.add(u,0),[-d,0],null),new k(o.subtract(u,0),null,[d,0]),new k(o.add(0,l),[0,-_],null),new k(h.subtract(0,l),null,[0,_]),new k(h.subtract(u,0),[d,0])]}return t(e,!0,arguments)},RoundRectangle:"#Rectangle",Ellipse:function(){var t=b._readEllipse(arguments);return n(t.center,t.radius,arguments)},Oval:"#Ellipse",Arc:function(){var t=h.readNamed(arguments,"from"),n=h.readNamed(arguments,"through"),i=h.readNamed(arguments,"to"),r=e.getNamed(arguments),s=new O(r&&r.insert===!1&&y.NO_INSERT);return s.moveTo(t),s.arcTo(n,i),s.set(r)},RegularPolygon:function(){for(var n=h.readNamed(arguments,"center"),i=e.readNamed(arguments,"sides"),r=e.readNamed(arguments,"radius"),s=360/i,a=!(i%3),o=new h(0,a?-r:r),u=a?-1:.5,l=Array(i),c=0;i>c;c++)l[c]=new k(n.add(o.rotate((c+u)*s)));return t(l,!0,arguments)},Star:function(){for(var n=h.readNamed(arguments,"center"),i=2*e.readNamed(arguments,"points"),r=e.readNamed(arguments,"radius1"),s=e.readNamed(arguments,"radius2"),a=360/i,o=new h(0,-1),u=Array(i),l=0;i>l;l++)u[l]=new k(n.add(o.rotate(a*l).multiply(l%2?s:r)));return t(u,!0,arguments)}}}});var T=A.extend({_class:"CompoundPath",_serializeFields:{children:[]},initialize:function(t){this._children=[],this._namedChildren={},this._initialize(t)||("string"==typeof t?this.setPathData(t):this.addChildren(Array.isArray(t)?t:arguments))},insertChildren:function we(e,n,i){n=we.base.call(this,e,n,i,O);for(var r=0,s=!i&&n&&n.length;s>r;r++){var a=n[r];a._clockwise===t&&a.setClockwise(0===a._index)}return n},reverse:function(){for(var t=this._children,e=0,n=t.length;n>e;e++)t[e].reverse()},smooth:function(){for(var t=0,e=this._children.length;e>t;t++)this._children[t].smooth()},isClockwise:function(){var t=this.getFirstChild();return t&&t.isClockwise()},setClockwise:function(t){this.isClockwise()!==!!t&&this.reverse()},getFirstSegment:function(){var t=this.getFirstChild();return t&&t.getFirstSegment()},getLastSegment:function(){var t=this.getLastChild();return t&&t.getLastSegment()},getCurves:function(){for(var t=this._children,e=[],n=0,i=t.length;i>n;n++)e.push.apply(e,t[n].getCurves());return e},getFirstCurve:function(){var t=this.getFirstChild();return t&&t.getFirstCurve()},getLastCurve:function(){var t=this.getLastChild();return t&&t.getFirstCurve()},getArea:function(){for(var t=this._children,e=0,n=0,i=t.length;i>n;n++)e+=t[n].getArea();return e}},{beans:!0,getPathData:function(t,e){for(var n=this._children,i=[],r=0,s=n.length;s>r;r++){var a=n[r],o=a._matrix;i.push(a.getPathData(t&&!o.isIdentity()?t.chain(o):o,e))}return i.join(" ")}},{_getChildHitTestOptions:function(t){return t.class===O||"path"===t.type?t:new e(t,{fill:!1})},_draw:function(t,e,n){var i=this._children;if(0!==i.length){if(this._currentPath)t.currentPath=this._currentPath;else{e=e.extend({dontStart:!0,dontFinish:!0}),t.beginPath();for(var r=0,s=i.length;s>r;r++)i[r].draw(t,e,n);this._currentPath=t.currentPath}if(!e.clip){this._setStyles(t);var a=this._style;a.hasFill()&&(t.fill(a.getWindingRule()),t.shadowColor="rgba(0,0,0,0)"),a.hasStroke()&&t.stroke()}}},_drawSelected:function(t,e,n){for(var i=this._children,r=0,s=i.length;s>r;r++){var a=i[r],o=a._matrix;n[a._id]||a._drawSelected(t,o.isIdentity()?e:e.chain(o))}}},new function(){function t(t,e){var n=t._children;if(e&&0===n.length)throw Error("Use a moveTo() command first");return n[n.length-1]}var n={moveTo:function(){var e=t(this),n=e&&e.isEmpty()?e:new O;n!==e&&this.addChild(n),n.moveTo.apply(n,arguments)},moveBy:function(){var e=t(this,!0),n=e&&e.getLastSegment(),i=h.read(arguments);this.moveTo(n?i.add(n._point):i)},closePath:function(e){t(this,!0).closePath(e)}};return e.each(["lineTo","cubicCurveTo","quadraticCurveTo","curveTo","arcTo","lineBy","cubicCurveBy","quadraticCurveBy","curveBy","arcBy"],function(e){n[e]=function(){var n=t(this,!0);n[e].apply(n,arguments)}}),n});A.inject(new function(){function t(t,r,s,a){function o(t){return t.clone(!1).reduce().reorient().transform(null,!0)}function h(t){for(var e=0,n=t.length;n>e;e++){var i=t[e];_.push.apply(_,i._segments),g.push.apply(g,i._getMonoCurves())}}var u=o(t),l=r&&t!==r&&o(r);u.isClockwise()||u.reverse(),!l||a^l.isClockwise()||l.reverse(),e(u.getIntersections(l,null,!0));var c=[],d=[],f=[],_=[],g=[];h(u._children||[u]),l&&h(l._children||[l]),_.sort(function(t,e){var n=t._intersection,i=e._intersection;return!n&&!i||n&&i?0:n?-1:1});for(var p=0,v=_.length;v>p;p++){var m=_[p];if(null==m._winding){c.length=d.length=f.length=0;var y=0,w=m;do c.push(m),f.push(y+=m.getCurve().getLength()),m=m.getNext();while(m&&!m._intersection&&m!==w);for(var x=0;3>x;x++){var b=y*Math.random(),C=f.length,S=0;do if(f[S]>=b){S>0&&(b-=f[S-1]);break}while(++S<C);var P=c[S].getCurve(),k=P.getPointAt(b),M=P.isHorizontal(),I=P._path;I._parent instanceof T&&(I=I._parent),d[x]=a&&l&&(I===u&&l._getWinding(k,M)||I===l&&!u._getWinding(k,M))?0:n(k,g,M)}d.sort();for(var z=d[1],x=c.length-1;x>=0;x--)c[x]._winding=z}}var A=new T;return A.addChildren(i(_,s),!0),u.remove(),l&&l.remove(),A=A.reduce(),A.setStyle(t._style),A}function e(t){function e(){for(var t=0,e=n.length;e>t;t++){var i=n[t];i._handleOut.set(0,0),i._handleIn.set(0,0)}}for(var n,i,r,s=1e-5,a=t.length-1;a>=0;a--){var o=t[a],h=o._parameter;r&&r._curve===o._curve&&r._parameter>0?h/=r._parameter:(n&&e(),i=o._curve,n=i.isLinear()&&[]);var u,l;(u=i.divide(h,!0,!0))?(l=u._segment1,i=u.getPrevious()):l=s>h?i._segment1:h>1-s?i._segment2:i.getPartLength(0,h)<i.getPartLength(h,1)?i._segment1:i._segment2,l._intersection=o.getIntersection(),o._segment=l,n&&n.push(l),r=o}n&&e()}function n(t,e,i,r){var s=1e-5,a=t.x,o=t.y,u=0,l=0,c=[],d=Math.abs,f=1-s;if(i){for(var _=-1/0,g=1/0,p=o-s,v=o+s,m=0,y=e.length;y>m;m++){var w=e[m].values;if(I.solveCubic(w,0,a,c,0,1)>0)for(var x=c.length-1;x>=0;x--){var b=I.evaluate(w,c[x],0).y;p>b&&b>_?_=b:b>v&&g>b&&(g=b)}}_=(_+o)/2,g=(g+o)/2,_>-1/0&&(u=n(new h(a,_),e)),1/0>g&&(l=n(new h(a,g),e))}else for(var C=a-s,S=a+s,m=0,y=e.length;y>m;m++){var P=e[m],w=P.values,k=P.winding,M=P.next;if(k&&(1===k&&o>=w[1]&&o<=w[7]||o>=w[7]&&o<=w[1])&&1===I.solveCubic(w,1,o,c,0,M.winding||M.values[1]!==o?f:1)){var z=c[0],A=I.evaluate(w,z,0).x,O=I.evaluate(w,z,1).y;d(O)<s&&!I.isLinear(w)||s>z&&O*I.evaluate(P.previous.values,z,1).y<0?r&&A>=C&&S>=A&&(++u,++l):C>=A?u+=k:A>=S&&(l+=k)}}return Math.max(d(u),d(l))}function i(t,e,n){e=e||function(){return!0};for(var i,r,s=[],a=.001,o=.999,h=0,u=t.length;u>h;h++)if(i=r=t[h],!i._visited&&e(i._winding)){var l=new O(y.NO_INSERT),c=i._intersection,d=c&&c._segment,f=!1,_=1;do{var g,p=_>0?i._handleIn:i._handleOut,v=_>0?i._handleOut:i._handleIn;if(f&&(!e(i._winding)||n)&&(c=i._intersection)&&(g=c._segment)&&g!==r){if(n)i._visited=g._visited,i=g,_=1;else{var m=i.getCurve();_>0&&(m=m.getPrevious());var w=m.getTangentAt(1>_?a:o,!0),x=g.getCurve(),b=x.getPrevious(),C=b.getTangentAt(o,!0),S=x.getTangentAt(a,!0),P=w.cross(C),M=w.cross(S);if(0!==P*M){var I=M>P?b:x,z=e(I._segment1._winding)?I:M>P?x:b,A=z._segment1;_=z===b?-1:1,A._visited&&i._path!==A._path||!e(A._winding)?_=1:(i._visited=g._visited,i=g,A._visited&&(_=1))}else _=1}v=_>0?i._handleOut:i._handleIn}l.add(new k(i._point,f&&p,v)),f=!0,i._visited=!0,i=_>0?i.getNext():i.getPrevious()}while(i&&!i._visited&&i!==r&&i!==d&&(i._intersection||e(i._winding)));!i||i!==r&&i!==d?l.lastSegment._handleOut.set(0,0):(l.firstSegment.setHandleIn((i===d?d:i)._handleIn),l.setClosed(!0)),l._segments.length>(l._closed?l.isPolygon()?2:0:1)&&s.push(l)}return s}return{_getWinding:function(t,e,i){return n(t,this._getMonoCurves(),e,i)},unite:function(e){return t(this,e,function(t){return 1===t||0===t},!1)},intersect:function(e){return t(this,e,function(t){return 2===t},!1)},subtract:function(e){return t(this,e,function(t){return 1===t},!0)},exclude:function(t){return new w([this.subtract(t),t.subtract(this)])},divide:function(t){return new w([this.subtract(t),this.intersect(t)])}}}),O.inject({_getMonoCurves:function(){function t(t){var e=t[1],r=t[7],s={values:t,winding:e===r?0:e>r?-1:1,previous:n,next:null};n&&(n.next=s),i.push(s),n=s}function e(e){if(0!==I.getLength(e)){var n=e[1],i=e[3],r=e[5],s=e[7];if(I.isLinear(e))t(e);else{var a=3*(i-r)-n+s,h=2*(n+r)-4*i,u=i-n,l=1e-5,c=[],d=o.solveQuadratic(a,h,u,c,l,1-l);if(0===d)t(e);else{c.sort();var f=c[0],_=I.subdivide(e,f);t(_[0]),d>1&&(f=(c[1]-f)/(1-f),_=I.subdivide(_[1],f),t(_[0])),t(_[1])}}}}var n,i=this._monoCurves;if(!i){i=this._monoCurves=[];for(var r=this.getCurves(),s=this._segments,a=0,h=r.length;h>a;a++)e(r[a].getValues());if(!this._closed&&s.length>1){var u=s[s.length-1]._point,l=s[0]._point,c=u._x,d=u._y,f=l._x,_=l._y;e([c,d,c,d,f,_,f,_])}if(i.length>0){var g=i[0],p=i[i.length-1];g.previous=p,p.next=g}}return i},getInteriorPoint:function(){var t=this.getBounds(),e=t.getCenter(!0);if(!this.contains(e)){for(var n=this._getMonoCurves(),i=[],r=e.y,s=[],a=0,o=n.length;o>a;a++){var h=n[a].values;if((1===n[a].winding&&r>=h[1]&&r<=h[7]||r>=h[7]&&r<=h[1])&&I.solveCubic(h,1,r,i,0,1)>0)for(var u=i.length-1;u>=0;u--)s.push(I.evaluate(h,i[u],0).x);if(s.length>1)break}e.x=(s[0]+s[1])/2}return e},reorient:function(){return this.setClockwise(!0),this}}),T.inject({_getMonoCurves:function(){for(var t=this._children,e=[],n=0,i=t.length;i>n;n++)e.push.apply(e,t[n]._getMonoCurves());return e},reorient:function(){var t=this.removeChildren().sort(function(t,e){return e.getBounds().getArea()-t.getBounds().getArea()});this.addChildren(t);for(var e=t[0].isClockwise(),n=1,i=t.length;i>n;n++){for(var r=t[n].getInteriorPoint(),s=0,a=n-1;a>=0;a--)t[a].contains(r)&&s++;t[n].setClockwise(0===s%2&&e)}return this}});var L=e.extend({_class:"PathIterator",initialize:function(t,e,n,i){function r(t,e){var n=I.getValues(t,e,i);o.push(n),s(n,t._index,0,1)}function s(t,e,i,r){if(r-i>l&&!I.isFlatEnough(t,n||.25)){var a=I.subdivide(t),o=(i+r)/2;s(a[0],e,i,o),s(a[1],e,o,r)}else{var c=t[6]-t[0],d=t[7]-t[1],f=Math.sqrt(c*c+d*d);f>1e-5&&(u+=f,h.push({offset:u,value:r,index:e}))}}for(var a,o=[],h=[],u=0,l=1/(e||32),c=t._segments,d=c[0],f=1,_=c.length;_>f;f++)a=c[f],r(d,a),d=a;t._closed&&r(a,c[0]),this.curves=o,this.parts=h,this.length=u,this.index=0},getParameterAt:function(t){for(var e,n=this.index;e=n,!(0==n||this.parts[--n].offset<t););for(var i=this.parts.length;i>e;e++){var r=this.parts[e];if(r.offset>=t){this.index=e;var s=this.parts[e-1],a=s&&s.index==r.index?s.value:0,o=s?s.offset:0;return{value:a+(r.value-a)*(t-o)/(r.offset-o),index:r.index}}}var r=this.parts[this.parts.length-1];return{value:1,index:r.index}},evaluate:function(t,e){var n=this.getParameterAt(t);return I.evaluate(this.curves[n.index],n.value,e)},drawPart:function(t,e,n){e=this.getParameterAt(e),n=this.getParameterAt(n);for(var i=e.index;i<=n.index;i++){var r=I.getPart(this.curves[i],i==e.index?e.value:0,i==n.index?n.value:1);i==e.index&&t.moveTo(r[0],r[1]),t.bezierCurveTo.apply(t,r.slice(2))}}},e.each(["getPoint","getTangent","getNormal","getCurvature"],function(t,e){this[t+"At"]=function(t){return this.evaluate(t,e)}},{})),E=e.extend({initialize:function(t,e){this.points=[];for(var n,i=t._segments,r=0,s=i.length;s>r;r++){var a=i[r].point.clone();n&&n.equals(a)||(this.points.push(a),n=a)}this.error=e},fit:function(){var t=this.points,e=t.length;
return this.segments=e>0?[new k(t[0])]:[],e>1&&this.fitCubic(0,e-1,t[1].subtract(t[0]).normalize(),t[e-2].subtract(t[e-1]).normalize()),this.segments},fitCubic:function(e,n,i,r){if(1==n-e){var s=this.points[e],a=this.points[n],o=s.getDistance(a)/3;return this.addCurve([s,s.add(i.normalize(o)),a.add(r.normalize(o)),a]),t}for(var h,u=this.chordLengthParameterize(e,n),l=Math.max(this.error,this.error*this.error),c=0;4>=c;c++){var d=this.generateBezier(e,n,u,i,r),f=this.findMaxError(e,n,d,u);if(f.error<this.error)return this.addCurve(d),t;if(h=f.index,f.error>=l)break;this.reparameterize(e,n,u,d),l=f.error}var _=this.points[h-1].subtract(this.points[h]),g=this.points[h].subtract(this.points[h+1]),p=_.add(g).divide(2).normalize();this.fitCubic(e,h,i,p),this.fitCubic(h,n,p.negate(),r)},addCurve:function(t){var e=this.segments[this.segments.length-1];e.setHandleOut(t[1].subtract(t[0])),this.segments.push(new k(t[3],t[2].subtract(t[3])))},generateBezier:function(t,e,n,i,r){for(var s=1e-11,a=this.points[t],o=this.points[e],h=[[0,0],[0,0]],u=[0,0],l=0,c=e-t+1;c>l;l++){var d=n[l],f=1-d,_=3*d*f,g=f*f*f,p=_*f,v=_*d,m=d*d*d,y=i.normalize(p),w=r.normalize(v),x=this.points[t+l].subtract(a.multiply(g+p)).subtract(o.multiply(v+m));h[0][0]+=y.dot(y),h[0][1]+=y.dot(w),h[1][0]=h[0][1],h[1][1]+=w.dot(w),u[0]+=y.dot(x),u[1]+=w.dot(x)}var b,C,S=h[0][0]*h[1][1]-h[1][0]*h[0][1];if(Math.abs(S)>s){var P=h[0][0]*u[1]-h[1][0]*u[0],k=u[0]*h[1][1]-u[1]*h[0][1];b=k/S,C=P/S}else{var M=h[0][0]+h[0][1],I=h[1][0]+h[1][1];b=C=Math.abs(M)>s?u[0]/M:Math.abs(I)>s?u[1]/I:0}var z=o.getDistance(a);return s*=z,(s>b||s>C)&&(b=C=z/3),[a,a.add(i.normalize(b)),o.add(r.normalize(C)),o]},reparameterize:function(t,e,n,i){for(var r=t;e>=r;r++)n[r-t]=this.findRoot(i,this.points[r],n[r-t])},findRoot:function(t,e,n){for(var i=[],r=[],s=0;2>=s;s++)i[s]=t[s+1].subtract(t[s]).multiply(3);for(var s=0;1>=s;s++)r[s]=i[s+1].subtract(i[s]).multiply(2);var a=this.evaluate(3,t,n),o=this.evaluate(2,i,n),h=this.evaluate(1,r,n),u=a.subtract(e),l=o.dot(o)+u.dot(h);return Math.abs(l)<1e-5?n:n-u.dot(o)/l},evaluate:function(t,e,n){for(var i=e.slice(),r=1;t>=r;r++)for(var s=0;t-r>=s;s++)i[s]=i[s].multiply(1-n).add(i[s+1].multiply(n));return i[0]},chordLengthParameterize:function(t,e){for(var n=[0],i=t+1;e>=i;i++)n[i-t]=n[i-t-1]+this.points[i].getDistance(this.points[i-1]);for(var i=1,r=e-t;r>=i;i++)n[i]/=n[r];return n},findMaxError:function(t,e,n,i){for(var r=Math.floor((e-t+1)/2),s=0,a=t+1;e>a;a++){var o=this.evaluate(3,n,i[a-t]),h=o.subtract(this.points[a]),u=h.x*h.x+h.y*h.y;u>=s&&(s=u,r=a)}return{error:s,index:r}}}),N=y.extend({_class:"TextItem",_boundsSelected:!0,_applyMatrix:!1,_canApplyMatrix:!1,_serializeFields:{content:null},_boundsGetter:"getBounds",initialize:function(n){this._content="",this._lines=[];var i=n&&e.isPlainObject(n)&&n.x===t&&n.y===t;this._initialize(i&&n,!i&&h.read(arguments))},_equals:function(t){return this._content===t._content},_clone:function xe(t){return t.setContent(this._content),xe.base.call(this,t)},getContent:function(){return this._content},setContent:function(t){this._content=""+t,this._lines=this._content.split(/\r\n|\n|\r/gm),this._changed(265)},isEmpty:function(){return!this._content},getCharacterStyle:"#getStyle",setCharacterStyle:"#setStyle",getParagraphStyle:"#getStyle",setParagraphStyle:"#setStyle"}),j=N.extend({_class:"PointText",initialize:function(){N.apply(this,arguments)},clone:function(t){return this._clone(new j(y.NO_INSERT),t)},getPoint:function(){var t=this._matrix.getTranslation();return new u(t.x,t.y,this,"setPoint")},setPoint:function(){var t=h.read(arguments);this.translate(t.subtract(this._matrix.getTranslation()))},_draw:function(t){if(this._content){this._setStyles(t);var e=this._style,n=this._lines,i=e.getLeading(),r=t.shadowColor;t.font=e.getFontStyle(),t.textAlign=e.getJustification();for(var s=0,a=n.length;a>s;s++){t.shadowColor=r;var o=n[s];e.hasFill()&&(t.fillText(o,0,0),t.shadowColor="rgba(0,0,0,0)"),e.hasStroke()&&t.strokeText(o,0,0),t.translate(0,i)}}},_getBounds:function(t,e){var n=this._style,i=this._lines,r=i.length,s=n.getJustification(),a=n.getLeading(),o=this.getView().getTextWidth(n.getFontStyle(),i),h=0;"left"!==s&&(h-=o/("center"===s?2:1));var u=new f(h,r?-.75*a:0,o,r*a);return e?e._transformBounds(u,u):u}}),B=e.extend(new function(){function t(t){var e,i=t.match(/^#(\w{1,2})(\w{1,2})(\w{1,2})$/);if(i){e=[0,0,0];for(var r=0;3>r;r++){var a=i[r+1];e[r]=parseInt(1==a.length?a+a:a,16)/255}}else if(i=t.match(/^rgba?\((.*)\)$/)){e=i[1].split(",");for(var r=0,o=e.length;o>r;r++){var a=+e[r];e[r]=3>r?a/255:a}}else{var h=s[t];if(!h){n||(n=K.getContext(1,1),n.globalCompositeOperation="copy"),n.fillStyle="rgba(0,0,0,0)",n.fillStyle=t,n.fillRect(0,0,1,1);var u=n.getImageData(0,0,1,1).data;h=s[t]=[u[0]/255,u[1]/255,u[2]/255]}e=h.slice()}return e}var n,i={gray:["gray"],rgb:["red","green","blue"],hsb:["hue","saturation","brightness"],hsl:["hue","saturation","lightness"],gradient:["gradient","origin","destination","highlight"]},r={},s={},o=[[0,3,1],[2,0,1],[1,0,3],[1,2,0],[3,1,0],[0,1,2]],u={"rgb-hsb":function(t,e,n){var i=Math.max(t,e,n),r=Math.min(t,e,n),s=i-r,a=0===s?0:60*(i==t?(e-n)/s+(n>e?6:0):i==e?(n-t)/s+2:(t-e)/s+4);return[a,0===i?0:s/i,i]},"hsb-rgb":function(t,e,n){t=(t/60%6+6)%6;var i=Math.floor(t),r=t-i,i=o[i],s=[n,n*(1-e),n*(1-e*r),n*(1-e*(1-r))];return[s[i[0]],s[i[1]],s[i[2]]]},"rgb-hsl":function(t,e,n){var i=Math.max(t,e,n),r=Math.min(t,e,n),s=i-r,a=0===s,o=a?0:60*(i==t?(e-n)/s+(n>e?6:0):i==e?(n-t)/s+2:(t-e)/s+4),h=(i+r)/2,u=a?0:.5>h?s/(i+r):s/(2-i-r);return[o,u,h]},"hsl-rgb":function(t,e,n){if(t=(t/360%1+1)%1,0===e)return[n,n,n];for(var i=[t+1/3,t,t-1/3],r=.5>n?n*(1+e):n+e-n*e,s=2*n-r,a=[],o=0;3>o;o++){var h=i[o];0>h&&(h+=1),h>1&&(h-=1),a[o]=1>6*h?s+6*(r-s)*h:1>2*h?r:2>3*h?s+6*(r-s)*(2/3-h):s}return a},"rgb-gray":function(t,e,n){return[.2989*t+.587*e+.114*n]},"gray-rgb":function(t){return[t,t,t]},"gray-hsb":function(t){return[0,0,t]},"gray-hsl":function(t){return[0,0,t]},"gradient-rgb":function(){return[]},"rgb-gradient":function(){return[]}};return e.each(i,function(t,n){r[n]=[],e.each(t,function(t,s){var a=e.capitalize(t),o=/^(hue|saturation)$/.test(t),u=r[n][s]="gradient"===t?function(t){var e=this._components[0];return t=D.read(Array.isArray(t)?t:arguments,0,{readNull:!0}),e!==t&&(e&&e._removeOwner(this),t&&t._addOwner(this)),t}:"gradient"===n?function(){return h.read(arguments,0,{readNull:"highlight"===t,clone:!0})}:function(t){return null==t||isNaN(t)?0:t};this["get"+a]=function(){return this._type===n||o&&/^hs[bl]$/.test(this._type)?this._components[s]:this._convert(n)[s]},this["set"+a]=function(t){this._type===n||o&&/^hs[bl]$/.test(this._type)||(this._components=this._convert(n),this._properties=i[n],this._type=n),t=u.call(this,t),null!=t&&(this._components[s]=t,this._changed())}},this)},{_class:"Color",_readIndex:!0,initialize:function l(e){var n,s,a,o,h=Array.prototype.slice,u=arguments,c=0;Array.isArray(e)&&(u=e,e=u[0]);var d=null!=e&&typeof e;if("string"===d&&e in i&&(n=e,e=u[1],Array.isArray(e)?(s=e,a=u[2]):(this.__read&&(c=1),u=h.call(u,1),d=typeof e)),!s){if(o="number"===d?u:"object"===d&&null!=e.length?e:null){n||(n=o.length>=3?"rgb":"gray");var f=i[n].length;a=o[f],this.__read&&(c+=o===arguments?f+(null!=a?1:0):1),o.length>f&&(o=h.call(o,0,f))}else if("string"===d)n="rgb",s=t(e),4===s.length&&(a=s[3],s.length--);else if("object"===d)if(e.constructor===l){if(n=e._type,s=e._components.slice(),a=e._alpha,"gradient"===n)for(var _=1,g=s.length;g>_;_++){var p=s[_];p&&(s[_]=p.clone())}}else if(e.constructor===D)n="gradient",o=u;else{n="hue"in e?"lightness"in e?"hsl":"hsb":"gradient"in e||"stops"in e||"radial"in e?"gradient":"gray"in e?"gray":"rgb";var v=i[n];y=r[n],this._components=s=[];for(var _=0,g=v.length;g>_;_++){var m=e[v[_]];null==m&&0===_&&"gradient"===n&&"stops"in e&&(m={stops:e.stops,radial:e.radial}),m=y[_].call(this,m),null!=m&&(s[_]=m)}a=e.alpha}this.__read&&n&&(c=1)}if(this._type=n||"rgb","gradient"===n&&(this._id=l._id=(l._id||0)+1),!s){this._components=s=[];for(var y=r[this._type],_=0,g=y.length;g>_;_++){var m=y[_].call(this,o&&o[_]);null!=m&&(s[_]=m)}}this._components=s,this._properties=i[this._type],this._alpha=a,this.__read&&(this.__read=c)},_serialize:function(t,n){var i=this.getComponents();return e.serialize(/^(gray|rgb)$/.test(this._type)?i:[this._type].concat(i),t,!0,n)},_changed:function(){this._canvasStyle=null,this._owner&&this._owner._changed(65)},_convert:function(t){var e;return this._type===t?this._components.slice():(e=u[this._type+"-"+t])?e.apply(this,this._components):u["rgb-"+t].apply(this,u[this._type+"-rgb"].apply(this,this._components))},convert:function(t){return new B(t,this._convert(t),this._alpha)},getType:function(){return this._type},setType:function(t){this._components=this._convert(t),this._properties=i[t],this._type=t},getComponents:function(){var t=this._components.slice();return null!=this._alpha&&t.push(this._alpha),t},getAlpha:function(){return null!=this._alpha?this._alpha:1},setAlpha:function(t){this._alpha=null==t?null:Math.min(Math.max(t,0),1),this._changed()},hasAlpha:function(){return null!=this._alpha},equals:function(t){var n=e.isPlainValue(t,!0)?B.read(arguments):t;return n===this||n&&this._class===n._class&&this._type===n._type&&this._alpha===n._alpha&&e.equals(this._components,n._components)||!1},toString:function(){for(var t=this._properties,e=[],n="gradient"===this._type,i=a.instance,r=0,s=t.length;s>r;r++){var o=this._components[r];null!=o&&e.push(t[r]+": "+(n?o:i.number(o)))}return null!=this._alpha&&e.push("alpha: "+i.number(this._alpha)),"{ "+e.join(", ")+" }"},toCSS:function(t){function e(t){return Math.round(255*(0>t?0:t>1?1:t))}var n=this._convert("rgb"),i=t||null==this._alpha?1:this._alpha;return n=[e(n[0]),e(n[1]),e(n[2])],1>i&&n.push(0>i?0:i),t?"#"+((1<<24)+(n[0]<<16)+(n[1]<<8)+n[2]).toString(16).slice(1):(4==n.length?"rgba(":"rgb(")+n.join(",")+")"},toCanvasStyle:function(t){if(this._canvasStyle)return this._canvasStyle;if("gradient"!==this._type)return this._canvasStyle=this.toCSS();var e,n=this._components,i=n[0],r=i._stops,s=n[1],a=n[2];if(i._radial){var o=a.getDistance(s),h=n[3];if(h){var u=h.subtract(s);u.getLength()>o&&(h=s.add(u.normalize(o-.1)))}var l=h||s;e=t.createRadialGradient(l.x,l.y,0,s.x,s.y,o)}else e=t.createLinearGradient(s.x,s.y,a.x,a.y);for(var c=0,d=r.length;d>c;c++){var f=r[c];e.addColorStop(f._rampPoint,f._color.toCanvasStyle())}return this._canvasStyle=e},transform:function(t){if("gradient"===this._type){for(var e=this._components,n=1,i=e.length;i>n;n++){var r=e[n];t._transformPoint(r,r,!0)}this._changed()}},statics:{_types:i,random:function(){var t=Math.random;return new B(t(),t(),t())}}})},new function(){var t={add:function(t,e){return t+e},subtract:function(t,e){return t-e},multiply:function(t,e){return t*e},divide:function(t,e){return t/e}};return e.each(t,function(t,e){this[e]=function(e){e=B.read(arguments);for(var n=this._type,i=this._components,r=e._convert(n),s=0,a=i.length;a>s;s++)r[s]=t(i[s],r[s]);return new B(n,r,null!=this._alpha?t(this._alpha,e.getAlpha()):null)}},{})});e.each(B._types,function(t,n){var i=this[e.capitalize(n)+"Color"]=function(t){var e=null!=t&&typeof t,i="object"===e&&null!=t.length?t:"string"===e?null:arguments;return i?new B(n,i):new B(t)};if(3==n.length){var r=n.toUpperCase();B[r]=this[r+"Color"]=i}},e.exports);var D=e.extend({_class:"Gradient",initialize:function be(t,e){this._id=be._id=(be._id||0)+1,t&&this._set(t)&&(t=e=null),this._stops||this.setStops(t||["white","black"]),null==this._radial&&this.setRadial("string"==typeof e&&"radial"===e||e||!1)},_serialize:function(t,n){return n.add(this,function(){return e.serialize([this._stops,this._radial],t,!0,n)})},_changed:function(){for(var t=0,e=this._owners&&this._owners.length;e>t;t++)this._owners[t]._changed()},_addOwner:function(t){this._owners||(this._owners=[]),this._owners.push(t)},_removeOwner:function(e){var n=this._owners?this._owners.indexOf(e):-1;-1!=n&&(this._owners.splice(n,1),0===this._owners.length&&(this._owners=t))},clone:function(){for(var t=[],e=0,n=this._stops.length;n>e;e++)t[e]=this._stops[e].clone();return new D(t)},getStops:function(){return this._stops},setStops:function(e){if(this.stops)for(var n=0,i=this._stops.length;i>n;n++)this._stops[n]._owner=t;if(e.length<2)throw Error("Gradient stop list needs to contain at least two stops.");this._stops=R.readAll(e,0,{clone:!0});for(var n=0,i=this._stops.length;i>n;n++){var r=this._stops[n];r._owner=this,r._defaultRamp&&r.setRampPoint(n/(i-1))}this._changed()},getRadial:function(){return this._radial},setRadial:function(t){this._radial=t,this._changed()},equals:function(t){if(t===this)return!0;if(t&&this._class===t._class&&this._stops.length===t._stops.length){for(var e=0,n=this._stops.length;n>e;e++)if(!this._stops[e].equals(t._stops[e]))return!1;return!0}return!1}}),R=e.extend({_class:"GradientStop",initialize:function(e,n){if(e){var i,r;n===t&&Array.isArray(e)?(i=e[0],r=e[1]):e.color?(i=e.color,r=e.rampPoint):(i=e,r=n),this.setColor(i),this.setRampPoint(r)}},clone:function(){return new R(this._color.clone(),this._rampPoint)},_serialize:function(t,n){return e.serialize([this._color,this._rampPoint],t,!0,n)},_changed:function(){this._owner&&this._owner._changed(65)},getRampPoint:function(){return this._rampPoint},setRampPoint:function(t){this._defaultRamp=null==t,this._rampPoint=t||0,this._changed()},getColor:function(){return this._color},setColor:function(t){this._color=B.read(arguments),this._color===t&&(this._color=t.clone()),this._color._owner=this,this._changed()},equals:function(t){return t===this||t&&this._class===t._class&&this._color.equals(t._color)&&this._rampPoint==t._rampPoint||!1}}),F=e.extend(new function(){var n={fillColor:t,strokeColor:t,strokeWidth:1,strokeCap:"butt",strokeJoin:"miter",strokeScaling:!0,miterLimit:10,dashOffset:0,dashArray:[],windingRule:"nonzero",shadowColor:t,shadowBlur:0,shadowOffset:new h,selectedColor:t,fontFamily:"sans-serif",fontWeight:"normal",fontSize:12,font:"sans-serif",leading:null,justification:"left"},i={strokeWidth:97,strokeCap:97,strokeJoin:97,strokeScaling:105,miterLimit:97,fontFamily:9,fontWeight:9,fontSize:9,font:9,leading:9,justification:9},r={beans:!0},s={_defaults:n,_textDefaults:new e(n,{fillColor:new B}),beans:!0};return e.each(n,function(n,a){var o=/Color$/.test(a),u="shadowOffset"===a,l=e.capitalize(a),c=i[a],d="set"+l,f="get"+l;s[d]=function(e){var n=this._owner,i=n&&n._children;if(i&&i.length>0&&!(n instanceof T))for(var r=0,s=i.length;s>r;r++)i[r]._style[d](e);else{var h=this._values[a];h!=e&&(o&&(h&&(h._owner=t),e&&e.constructor===B&&(e._owner&&(e=e.clone()),e._owner=n)),this._values[a]=e,n&&n._changed(c||65))}},s[f]=function(n){var i,r=this._owner,s=r&&r._children;if(!s||0===s.length||n||r instanceof T){var i=this._values[a];if(i===t)i=this._defaults[a],i&&i.clone&&(i=i.clone()),this._values[a]=i;else{var l=o?B:u?h:null;!l||i&&i.constructor===l||(this._values[a]=i=l.read([i],0,{readNull:!0,clone:!0}),i&&o&&(i._owner=r))}return i}for(var c=0,d=s.length;d>c;c++){var _=s[c]._style[f]();if(0===c)i=_;else if(!e.equals(i,_))return t}return i},r[f]=function(t){return this._style[f](t)},r[d]=function(t){this._style[d](t)}}),y.inject(r),s},{_class:"Style",initialize:function(t,e,n){this._values={},this._owner=e,this._project=e&&e._project||n||paper.project,e instanceof N&&(this._defaults=this._textDefaults),t&&this.set(t)},set:function(t){var e=t instanceof F,n=e?t._values:t;if(n)for(var i in n)if(i in this._defaults){var r=n[i];this[i]=r&&e&&r.clone?r.clone():r}},equals:function(t){return t===this||t&&this._class===t._class&&e.equals(this._values,t._values)||!1},hasFill:function(){return!!this.getFillColor()},hasStroke:function(){return!!this.getStrokeColor()&&this.getStrokeWidth()>0},hasShadow:function(){return!!this.getShadowColor()&&this.getShadowBlur()>0},getView:function(){return this._project.getView()},getFontStyle:function(){var t=this.getFontSize();return this.getFontWeight()+" "+t+(/[a-z]/i.test(t+"")?" ":"px ")+this.getFontFamily()},getFont:"#getFontFamily",setFont:"#setFontFamily",getLeading:function Ce(){var t=Ce.base.call(this),e=this.getFontSize();return/pt|em|%|px/.test(e)&&(e=this.getView().getPixelSize(e)),null!=t?t:1.2*e}}),q=new function(){function t(t,e,n,i){for(var r=["","webkit","moz","Moz","ms","o"],s=e[0].toUpperCase()+e.substring(1),a=0;6>a;a++){var o=r[a],h=o?o+s:e;if(h in t){if(!n)return t[h];t[h]=i;break}}}return{getStyles:function(t){var e=t&&9!==t.nodeType?t.ownerDocument:t,n=e&&e.defaultView;return n&&n.getComputedStyle(t,"")},getBounds:function(t,e){var n,i=t.ownerDocument,r=i.body,s=i.documentElement;try{n=t.getBoundingClientRect()}catch(a){n={left:0,top:0,width:0,height:0}}var o=n.left-(s.clientLeft||r.clientLeft||0),h=n.top-(s.clientTop||r.clientTop||0);if(!e){var u=i.defaultView;o+=u.pageXOffset||s.scrollLeft||r.scrollLeft,h+=u.pageYOffset||s.scrollTop||r.scrollTop}return new f(o,h,n.width,n.height)},getViewportBounds:function(t){var e=t.ownerDocument,n=e.defaultView,i=e.documentElement;return new f(0,0,n.innerWidth||i.clientWidth,n.innerHeight||i.clientHeight)},getOffset:function(t,e){return q.getBounds(t,e).getPoint()},getSize:function(t){return q.getBounds(t,!0).getSize()},isInvisible:function(t){return q.getSize(t).equals(new c(0,0))},isInView:function(t){return!q.isInvisible(t)&&q.getViewportBounds(t).intersects(q.getBounds(t,!0))},getPrefixed:function(e,n){return t(e,n)},setPrefixed:function(e,n,i){if("object"==typeof n)for(var r in n)t(e,r,!0,n[r]);else t(e,n,!0,i)}}},V={add:function(t,e){for(var n in e)for(var i=e[n],r=n.split(/[\s,]+/g),s=0,a=r.length;a>s;s++)t.addEventListener(r[s],i,!1)},remove:function(t,e){for(var n in e)for(var i=e[n],r=n.split(/[\s,]+/g),s=0,a=r.length;a>s;s++)t.removeEventListener(r[s],i,!1)},getPoint:function(t){var e=t.targetTouches?t.targetTouches.length?t.targetTouches[0]:t.changedTouches[0]:t;return new h(e.pageX||e.clientX+document.documentElement.scrollLeft,e.pageY||e.clientY+document.documentElement.scrollTop)},getTarget:function(t){return t.target||t.srcElement},getRelatedTarget:function(t){return t.relatedTarget||t.toElement},getOffset:function(t,e){return V.getPoint(t).subtract(q.getOffset(e||V.getTarget(t)))},stop:function(t){t.stopPropagation(),t.preventDefault()}};V.requestAnimationFrame=new function(){function t(){for(var e=s.length-1;e>=0;e--){var o=s[e],h=o[0],u=o[1];(!u||("true"==r.getAttribute(u,"keepalive")||a)&&q.isInView(u))&&(s.splice(e,1),h())}n&&(s.length?n(t):i=!1)}var e,n=q.getPrefixed(window,"requestAnimationFrame"),i=!1,s=[],a=!0;return V.add(window,{focus:function(){a=!0},blur:function(){a=!1}}),function(r,a){s.push([r,a]),n?i||(n(t),i=!0):e||(e=setInterval(t,1e3/60))}};var Z=e.extend(n,{_class:"View",initialize:function Se(t,e){this._project=t,this._scope=t._scope,this._element=e;var n;this._pixelRatio||(this._pixelRatio=window.devicePixelRatio||1),this._id=e.getAttribute("id"),null==this._id&&e.setAttribute("id",this._id="view-"+Se._id++),V.add(e,this._viewEvents);var i="none";if(q.setPrefixed(e.style,{userSelect:i,touchAction:i,touchCallout:i,contentZooming:i,userDrag:i,tapHighlightColor:"rgba(0,0,0,0)"}),r.hasAttribute(e,"resize")){var s=q.getOffset(e,!0),a=this;n=q.getViewportBounds(e).getSize().subtract(s),this._windowEvents={resize:function(){q.isInvisible(e)||(s=q.getOffset(e,!0)),a.setViewSize(q.getViewportBounds(e).getSize().subtract(s))}},V.add(window,this._windowEvents)}else if(n=q.getSize(e),n.isNaN()||n.isZero()){var o=function(t){return e[t]||parseInt(e.getAttribute(t),10)};n=new c(o("width"),o("height"))}if(this._setViewSize(n),r.hasAttribute(e,"stats")&&"undefined"!=typeof Stats){this._stats=new Stats;var h=this._stats.domElement,u=h.style,s=q.getOffset(e);u.position="absolute",u.left=s.x+"px",u.top=s.y+"px",document.body.appendChild(h)}Se._views.push(this),Se._viewsById[this._id]=this,this._viewSize=n,(this._matrix=new g)._owner=this,this._zoom=1,Se._focused||(Se._focused=this),this._frameItems={},this._frameItemCount=0},remove:function(){return this._project?(Z._focused===this&&(Z._focused=null),Z._views.splice(Z._views.indexOf(this),1),delete Z._viewsById[this._id],this._project._view===this&&(this._project._view=null),V.remove(this._element,this._viewEvents),V.remove(window,this._windowEvents),this._element=this._project=null,this.off("frame"),this._animate=!1,this._frameItems={},!0):!1},_events:{onFrame:{install:function(){this.play()},uninstall:function(){this.pause()}},onResize:{}},_animate:!1,_time:0,_count:0,_requestFrame:function(){var t=this;V.requestAnimationFrame(function(){t._requested=!1,t._animate&&(t._requestFrame(),t._handleFrame())},this._element),this._requested=!0},_handleFrame:function(){paper=this._scope;var t=Date.now()/1e3,n=this._before?t-this._before:0;this._before=t,this._handlingFrame=!0,this.emit("frame",new e({delta:n,time:this._time+=n,count:this._count++})),this._stats&&this._stats.update(),this._handlingFrame=!1,this.update()},_animateItem:function(t,e){var n=this._frameItems;e?(n[t._id]={item:t,time:0,count:0},1===++this._frameItemCount&&this.on("frame",this._handleFrameItems)):(delete n[t._id],0===--this._frameItemCount&&this.off("frame",this._handleFrameItems))},_handleFrameItems:function(t){for(var n in this._frameItems){var i=this._frameItems[n];i.item.emit("frame",new e(t,{time:i.time+=t.delta,count:i.count++}))}},_update:function(){this._project._needsUpdate=!0,this._handlingFrame||(this._animate?this._handleFrame():this.update())},_changed:function(t){1&t&&(this._project._needsUpdate=!0)},_transform:function(t){this._matrix.concatenate(t),this._bounds=null,this._update()},getElement:function(){return this._element},getPixelRatio:function(){return this._pixelRatio},getResolution:function(){return 72*this._pixelRatio},getViewSize:function(){var t=this._viewSize;return new d(t.width,t.height,this,"setViewSize")},setViewSize:function(){var t=c.read(arguments),e=t.subtract(this._viewSize);e.isZero()||(this._viewSize.set(t.width,t.height),this._setViewSize(t),this._bounds=null,this.emit("resize",{size:t,delta:e}),this._update())},_setViewSize:function(t){var e=this._element;e.width=t.width,e.height=t.height},getBounds:function(){return this._bounds||(this._bounds=this._matrix.inverted()._transformBounds(new f(new h,this._viewSize))),this._bounds},getSize:function(){return this.getBounds().getSize()},getCenter:function(){return this.getBounds().getCenter()},setCenter:function(){var t=h.read(arguments);this.scrollBy(t.subtract(this.getCenter()))},getZoom:function(){return this._zoom},setZoom:function(t){this._transform((new g).scale(t/this._zoom,this.getCenter())),this._zoom=t},isVisible:function(){return q.isInView(this._element)},scrollBy:function(){this._transform((new g).translate(h.read(arguments).negate()))},play:function(){this._animate=!0,this._requested||this._requestFrame()},pause:function(){this._animate=!1},draw:function(){this.update()},projectToView:function(){return this._matrix._transformPoint(h.read(arguments))},viewToProject:function(){return this._matrix._inverseTransform(h.read(arguments))}},{statics:{_views:[],_viewsById:{},_id:0,create:function(t,e){return"string"==typeof e&&(e=document.getElementById(e)),new U(t,e)}}},new function(){function t(t){var e=V.getTarget(t);return e.getAttribute&&Z._viewsById[e.getAttribute("id")]}function e(t,e){return t.viewToProject(V.getOffset(e,t._element))}function n(){if(!Z._focused||!Z._focused.isVisible())for(var t=0,e=Z._views.length;e>t;t++){var n=Z._views[t];if(n&&n.isVisible()){Z._focused=a=n;break}}}function i(t,e,n){t._handleEvent("mousemove",e,n);var i=t._scope.tool;return i&&i._handleEvent(l&&i.responds("mousedrag")?"mousedrag":"mousemove",e,n),t.update(),i}var r,s,a,o,h,u,l=!1,c=window.navigator;c.pointerEnabled||c.msPointerEnabled?(o="pointerdown MSPointerDown",h="pointermove MSPointerMove",u="pointerup pointercancel MSPointerUp MSPointerCancel"):(o="touchstart",h="touchmove",u="touchend touchcancel","ontouchstart"in window&&c.userAgent.match(/mobile|tablet|ip(ad|hone|od)|android|silk/i)||(o+=" mousedown",h+=" mousemove",u+=" mouseup"));var d={"selectstart dragstart":function(t){l&&t.preventDefault()}},f={mouseout:function(t){var n=Z._focused,r=V.getRelatedTarget(t);!n||r&&"HTML"!==r.nodeName||i(n,e(n,t),t)},scroll:n};return d[o]=function(n){var i=Z._focused=t(n),s=e(i,n);l=!0,i._handleEvent("mousedown",s,n),(r=i._scope.tool)&&r._handleEvent("mousedown",s,n),i.update()},f[h]=function(o){var h=Z._focused;if(!l){var u=t(o);u?(h!==u&&i(h,e(h,o),o),s=h,h=Z._focused=a=u):a&&a===h&&(h=Z._focused=s,n())}if(h){var c=e(h,o);(l||h.getBounds().contains(c))&&(r=i(h,c,o))}},f[u]=function(t){var n=Z._focused;if(n&&l){var i=e(n,t);l=!1,n._handleEvent("mouseup",i,t),r&&r._handleEvent("mouseup",i,t),n.update()}},V.add(document,f),V.add(window,{load:n}),{_viewEvents:d,_handleEvent:function(){},statics:{updateFocus:n}}}),U=Z.extend({_class:"CanvasView",initialize:function(t,e){if(!(e instanceof HTMLCanvasElement)){var n=c.read(arguments);if(n.isZero())throw Error("Cannot create CanvasView with the provided argument: "+[].slice.call(arguments,1));e=K.getCanvas(n)}if(this._context=e.getContext("2d"),this._eventCounters={},this._pixelRatio=1,!/^off|false$/.test(r.getAttribute(e,"hidpi"))){var i=window.devicePixelRatio||1,s=q.getPrefixed(this._context,"backingStorePixelRatio")||1;this._pixelRatio=i/s}Z.call(this,t,e)},_setViewSize:function(t){var e=t.width,n=t.height,i=this._pixelRatio,r=this._element,s=r.style;r.width=e*i,r.height=n*i,1!==i&&(s.width=e+"px",s.height=n+"px",this._context.scale(i,i))},getPixelSize:function(t){var e=this._context,n=e.font;return e.font=t+" serif",t=parseFloat(e.font),e.font=n,t},getTextWidth:function(t,e){var n=this._context,i=n.font,r=0;n.font=t;for(var s=0,a=e.length;a>s;s++)r=Math.max(r,n.measureText(e[s]).width);return n.font=i,r},update:function(){var t=this._project;if(!t||!t._needsUpdate)return!1;var e=this._context,n=this._viewSize;return e.clearRect(0,0,n.width+1,n.height+1),t.draw(e,this._matrix,this._pixelRatio),t._needsUpdate=!1,!0}},new function(){function e(e,n,i,r,s,a){function o(e){return e.responds(n)&&(h||(h=new $(n,i,r,s,a?r.subtract(a):null)),e.emit(n,h)&&h.isStopped)?(i.preventDefault(),!0):t}for(var h,u=s;u;){if(o(u))return!0;u=u.getParent()}return o(e)?!0:!1}var n,i,r,s,a,o,h,u,l;return{_handleEvent:function(t,c,d){if(this._eventCounters[t]){var f=this._project,_=f.hitTest(c,{tolerance:0,fill:!0,stroke:!0}),g=_&&_.item,p=!1;switch(t){case"mousedown":for(p=e(this,t,d,c,g),u=a==g&&Date.now()-l<300,s=a=g,n=i=r=c,h=!p&&g;h&&!h.responds("mousedrag");)h=h._parent;break;case"mouseup":p=e(this,t,d,c,g,n),h&&(i&&!i.equals(c)&&e(this,"mousedrag",d,c,h,i),g!==h&&(r=c,e(this,"mousemove",d,c,g,r))),!p&&g&&g===s&&(l=Date.now(),e(this,u&&s.responds("doubleclick")?"doubleclick":"click",d,n,g),u=!1),s=h=null;break;case"mousemove":h&&(p=e(this,"mousedrag",d,c,h,i)),p||(g!==o&&(r=c),p=e(this,t,d,c,g,r)),i=r=c,g!==o&&(e(this,"mouseleave",d,c,o),o=g,e(this,"mouseenter",d,c,g))}return p}}}}),H=e.extend({_class:"Event",initialize:function(t){this.event=t},isPrevented:!1,isStopped:!1,preventDefault:function(){this.isPrevented=!0,this.event.preventDefault()},stopPropagation:function(){this.isStopped=!0,this.event.stopPropagation()},stop:function(){this.stopPropagation(),this.preventDefault()},getModifiers:function(){return G.modifiers}}),W=H.extend({_class:"KeyEvent",initialize:function(t,e,n,i){H.call(this,i),this.type=t?"keydown":"keyup",this.key=e,this.character=n},toString:function(){return"{ type: '"+this.type+"', key: '"+this.key+"', character: '"+this.character+"', modifiers: "+this.getModifiers()+" }"}}),G=new function(){function t(t,n,r,h){var u,l=r?String.fromCharCode(r):"",c=i[n],d=c||l.toLowerCase(),f=t?"keydown":"keyup",_=Z._focused,g=_&&_.isVisible()&&_._scope,p=g&&g.tool;o[d]=t,c&&(u=e.camelize(c))in s&&(s[u]=t),t?a[n]=r:delete a[n],p&&p.responds(f)&&(paper=g,p.emit(f,new W(t,d,l,h)),_&&_.update())}var n,i={8:"backspace",9:"tab",13:"enter",16:"shift",17:"control",18:"option",19:"pause",20:"caps-lock",27:"escape",32:"space",35:"end",36:"home",37:"left",38:"up",39:"right",40:"down",46:"delete",91:"command",93:"command",224:"command"},r={9:!0,13:!0,32:!0},s=new e({shift:!1,control:!1,option:!1,command:!1,capsLock:!1,space:!1}),a={},o={};return V.add(document,{keydown:function(e){var a=e.which||e.keyCode;a in i||s.command?t(!0,a,a in r||s.command?a:0,e):n=a},keypress:function(e){null!=n&&(t(!0,n,e.which||e.keyCode,e),n=null)},keyup:function(e){var n=e.which||e.keyCode;n in a&&t(!1,n,a[n],e)}}),V.add(window,{blur:function(e){for(var n in a)t(!1,n,a[n],e)}}),{modifiers:s,isDown:function(t){return!!o[t]}}},$=H.extend({_class:"MouseEvent",initialize:function(t,e,n,i,r){H.call(this,e),this.type=t,this.point=n,this.target=i,this.delta=r},toString:function(){return"{ type: '"+this.type+"', point: "+this.point+", target: "+this.target+(this.delta?", delta: "+this.delta:"")+", modifiers: "+this.getModifiers()+" }"}}),X=H.extend({_class:"ToolEvent",_item:null,initialize:function(t,e,n){this.tool=t,this.type=e,this.event=n},_choosePoint:function(t,e){return t?t:e?e.clone():null},getPoint:function(){return this._choosePoint(this._point,this.tool._point)},setPoint:function(t){this._point=t},getLastPoint:function(){return this._choosePoint(this._lastPoint,this.tool._lastPoint)},setLastPoint:function(t){this._lastPoint=t},getDownPoint:function(){return this._choosePoint(this._downPoint,this.tool._downPoint)},setDownPoint:function(t){this._downPoint=t},getMiddlePoint:function(){return!this._middlePoint&&this.tool._lastPoint?this.tool._point.add(this.tool._lastPoint).divide(2):this._middlePoint},setMiddlePoint:function(t){this._middlePoint=t},getDelta:function(){return!this._delta&&this.tool._lastPoint?this.tool._point.subtract(this.tool._lastPoint):this._delta},setDelta:function(t){this._delta=t},getCount:function(){return/^mouse(down|up)$/.test(this.type)?this.tool._downCount:this.tool._count},setCount:function(t){this.tool[/^mouse(down|up)$/.test(this.type)?"downCount":"count"]=t},getItem:function(){if(!this._item){var t=this.tool._scope.project.hitTest(this.getPoint());if(t){for(var e=t.item,n=e._parent;/^(Group|CompoundPath)$/.test(n._class);)e=n,n=n._parent;this._item=e}}return this._item},setItem:function(t){this._item=t},toString:function(){return"{ type: "+this.type+", point: "+this.getPoint()+", count: "+this.getCount()+", modifiers: "+this.getModifiers()+" }"}}),J=s.extend({_class:"Tool",_list:"tools",_reference:"tool",_events:["onActivate","onDeactivate","onEditOptions","onMouseDown","onMouseUp","onMouseDrag","onMouseMove","onKeyDown","onKeyUp"],initialize:function(t){s.call(this),this._firstMove=!0,this._count=0,this._downCount=0,this._set(t)},getMinDistance:function(){return this._minDistance},setMinDistance:function(t){this._minDistance=t,null!=this._minDistance&&null!=this._maxDistance&&this._minDistance>this._maxDistance&&(this._maxDistance=this._minDistance)},getMaxDistance:function(){return this._maxDistance},setMaxDistance:function(t){this._maxDistance=t,null!=this._minDistance&&null!=this._maxDistance&&this._maxDistance<this._minDistance&&(this._minDistance=t)},getFixedDistance:function(){return this._minDistance==this._maxDistance?this._minDistance:null},setFixedDistance:function(t){this._minDistance=t,this._maxDistance=t},_updateEvent:function(t,e,n,i,r,s,a){if(!r){if(null!=n||null!=i){var o=null!=n?n:0,h=e.subtract(this._point),u=h.getLength();if(o>u)return!1;var l=null!=i?i:0;if(0!=l)if(u>l)e=this._point.add(h.normalize(l));else if(a)return!1}if(s&&e.equals(this._point))return!1}switch(this._lastPoint=r&&"mousemove"==t?e:this._point,this._point=e,t){case"mousedown":this._lastPoint=this._downPoint,this._downPoint=this._point,this._downCount++;break;case"mouseup":this._lastPoint=this._downPoint}return this._count=r?0:this._count+1,!0},_fireEvent:function(t,e){var n=paper.project._removeSets;if(n){"mouseup"===t&&(n.mousedrag=null);
var i=n[t];if(i){for(var r in i){var s=i[r];for(var a in n){var o=n[a];o&&o!=i&&delete o[s._id]}s.remove()}n[t]=null}}return this.responds(t)&&this.emit(t,new X(this,t,e))},_handleEvent:function(t,e,n){paper=this._scope;var i=!1;switch(t){case"mousedown":this._updateEvent(t,e,null,null,!0,!1,!1),i=this._fireEvent(t,n);break;case"mousedrag":for(var r=!1,s=!1;this._updateEvent(t,e,this.minDistance,this.maxDistance,!1,r,s);)i=this._fireEvent(t,n)||i,r=!0,s=!0;break;case"mouseup":!e.equals(this._point)&&this._updateEvent("mousedrag",e,this.minDistance,this.maxDistance,!1,!1,!1)&&(i=this._fireEvent("mousedrag",n)),this._updateEvent(t,e,null,this.maxDistance,!1,!1,!1),i=this._fireEvent(t,n)||i,this._updateEvent(t,e,null,null,!0,!1,!1),this._firstMove=!0;break;case"mousemove":for(;this._updateEvent(t,e,this.minDistance,this.maxDistance,this._firstMove,!0,!1);)i=this._fireEvent(t,n)||i,this._firstMove=!1}return i&&n.preventDefault(),i}}),Y={request:function(t,e,n){var i=new(window.ActiveXObject||XMLHttpRequest)("Microsoft.XMLHTTP");return i.open(t.toUpperCase(),e,!0),"overrideMimeType"in i&&i.overrideMimeType("text/plain"),i.onreadystatechange=function(){if(4===i.readyState){var t=i.status;if(0!==t&&200!==t)throw Error("Could not load "+e+" (Error "+t+")");n.call(i,i.responseText)}},i.send(null)}},K={canvases:[],getCanvas:function(t,e){var n,i=!0;"object"==typeof t&&(e=t.height,t=t.width),n=this.canvases.length?this.canvases.pop():document.createElement("canvas");var r=n.getContext("2d");return n.width===t&&n.height===e?i&&r.clearRect(0,0,t+1,e+1):(n.width=t,n.height=e),r.save(),n},getContext:function(t,e){return this.getCanvas(t,e).getContext("2d")},release:function(t){var e=t.canvas?t.canvas:t;e.getContext("2d").restore(),this.canvases.push(e)}},Q=new function(){function t(t,e,n){return.2989*t+.587*e+.114*n}function n(e,n,i,r){var s=r-t(e,n,i);f=e+s,_=n+s,g=i+s;var r=t(f,_,g),a=p(f,_,g),o=v(f,_,g);if(0>a){var h=r-a;f=r+(f-r)*r/h,_=r+(_-r)*r/h,g=r+(g-r)*r/h}if(o>255){var u=255-r,l=o-r;f=r+(f-r)*u/l,_=r+(_-r)*u/l,g=r+(g-r)*u/l}}function i(t,e,n){return v(t,e,n)-p(t,e,n)}function r(t,e,n,i){var r,s=[t,e,n],a=v(t,e,n),o=p(t,e,n);o=o===t?0:o===e?1:2,a=a===t?0:a===e?1:2,r=0===p(o,a)?1===v(o,a)?2:1:0,s[a]>s[o]?(s[r]=(s[r]-s[o])*i/(s[a]-s[o]),s[a]=i):s[r]=s[a]=0,s[o]=0,f=s[0],_=s[1],g=s[2]}var s,a,o,h,u,l,c,d,f,_,g,p=Math.min,v=Math.max,m=Math.abs,y={multiply:function(){f=u*s/255,_=l*a/255,g=c*o/255},screen:function(){f=u+s-u*s/255,_=l+a-l*a/255,g=c+o-c*o/255},overlay:function(){f=128>u?2*u*s/255:255-2*(255-u)*(255-s)/255,_=128>l?2*l*a/255:255-2*(255-l)*(255-a)/255,g=128>c?2*c*o/255:255-2*(255-c)*(255-o)/255},"soft-light":function(){var t=s*u/255;f=t+u*(255-(255-u)*(255-s)/255-t)/255,t=a*l/255,_=t+l*(255-(255-l)*(255-a)/255-t)/255,t=o*c/255,g=t+c*(255-(255-c)*(255-o)/255-t)/255},"hard-light":function(){f=128>s?2*s*u/255:255-2*(255-s)*(255-u)/255,_=128>a?2*a*l/255:255-2*(255-a)*(255-l)/255,g=128>o?2*o*c/255:255-2*(255-o)*(255-c)/255},"color-dodge":function(){f=0===u?0:255===s?255:p(255,255*u/(255-s)),_=0===l?0:255===a?255:p(255,255*l/(255-a)),g=0===c?0:255===o?255:p(255,255*c/(255-o))},"color-burn":function(){f=255===u?255:0===s?0:v(0,255-255*(255-u)/s),_=255===l?255:0===a?0:v(0,255-255*(255-l)/a),g=255===c?255:0===o?0:v(0,255-255*(255-c)/o)},darken:function(){f=s>u?u:s,_=a>l?l:a,g=o>c?c:o},lighten:function(){f=u>s?u:s,_=l>a?l:a,g=c>o?c:o},difference:function(){f=u-s,0>f&&(f=-f),_=l-a,0>_&&(_=-_),g=c-o,0>g&&(g=-g)},exclusion:function(){f=u+s*(255-u-u)/255,_=l+a*(255-l-l)/255,g=c+o*(255-c-c)/255},hue:function(){r(s,a,o,i(u,l,c)),n(f,_,g,t(u,l,c))},saturation:function(){r(u,l,c,i(s,a,o)),n(f,_,g,t(u,l,c))},luminosity:function(){n(u,l,c,t(s,a,o))},color:function(){n(s,a,o,t(u,l,c))},add:function(){f=p(u+s,255),_=p(l+a,255),g=p(c+o,255)},subtract:function(){f=v(u-s,0),_=v(l-a,0),g=v(c-o,0)},average:function(){f=(u+s)/2,_=(l+a)/2,g=(c+o)/2},negation:function(){f=255-m(255-s-u),_=255-m(255-a-l),g=255-m(255-o-c)}},w=this.nativeModes=e.each(["source-over","source-in","source-out","source-atop","destination-over","destination-in","destination-out","destination-atop","lighter","darker","copy","xor"],function(t){this[t]=!0},{}),x=K.getContext(1,1);e.each(y,function(t,e){var n="darken"===e,i=!1;x.save();try{x.fillStyle=n?"#300":"#a00",x.fillRect(0,0,1,1),x.globalCompositeOperation=e,x.globalCompositeOperation===e&&(x.fillStyle=n?"#a00":"#300",x.fillRect(0,0,1,1),i=x.getImageData(0,0,1,1).data[0]!==n?170:51)}catch(r){}x.restore(),w[e]=i}),K.release(x),this.process=function(t,e,n,i,r){var p=e.canvas,v="normal"===t;if(v||w[t])n.save(),n.setTransform(1,0,0,1,0,0),n.globalAlpha=i,v||(n.globalCompositeOperation=t),n.drawImage(p,r.x,r.y),n.restore();else{var m=y[t];if(!m)return;for(var x=n.getImageData(r.x,r.y,p.width,p.height),b=x.data,C=e.getImageData(0,0,p.width,p.height).data,S=0,P=b.length;P>S;S+=4){s=C[S],u=b[S],a=C[S+1],l=b[S+1],o=C[S+2],c=b[S+2],h=C[S+3],d=b[S+3],m();var k=h*i/255,M=1-k;b[S]=k*f+M*u,b[S+1]=k*_+M*l,b[S+2]=k*g+M*c,b[S+3]=h*i+M*d}n.putImageData(x,r.x,r.y)}}},te=e.each({fillColor:["fill","color"],strokeColor:["stroke","color"],strokeWidth:["stroke-width","number"],strokeCap:["stroke-linecap","string"],strokeJoin:["stroke-linejoin","string"],strokeScaling:["vector-effect","lookup",{"true":"none","false":"non-scaling-stroke"},function(t,e){return!e&&(t instanceof A||t instanceof b||t instanceof N)}],miterLimit:["stroke-miterlimit","number"],dashArray:["stroke-dasharray","array"],dashOffset:["stroke-dashoffset","number"],fontFamily:["font-family","string"],fontWeight:["font-weight","string"],fontSize:["font-size","number"],justification:["text-anchor","lookup",{left:"start",center:"middle",right:"end"}],opacity:["opacity","number"],blendMode:["mix-blend-mode","string"]},function(t,n){var i=e.capitalize(n),r=t[2];this[n]={type:t[1],property:n,attribute:t[0],toSVG:r,fromSVG:r&&e.each(r,function(t,e){this[t]=e},{}),exportFilter:t[3],get:"get"+i,set:"set"+i}},{}),ee={href:"http://www.w3.org/1999/xlink",xlink:"http://www.w3.org/2000/xmlns"};return new function(){function t(t,e){for(var n in e){var i=e[n],r=ee[n];"number"==typeof i&&(i=S.number(i)),r?t.setAttributeNS(r,n,i):t.setAttribute(n,i)}return t}function n(e,n){return t(document.createElementNS("http://www.w3.org/2000/svg",e),n)}function r(t,n,i){var r=new e,s=t.getTranslation();if(n){t=t.shiftless();var a=t._inverseTransform(s);r[i?"cx":"x"]=a.x,r[i?"cy":"y"]=a.y,s=null}if(!t.isIdentity()){var h=t.decompose();if(h&&!h.shearing){var u=[],l=h.rotation,c=h.scaling;s&&!s.isZero()&&u.push("translate("+S.point(s)+")"),l&&u.push("rotate("+S.number(l)+")"),o.isZero(c.x-1)&&o.isZero(c.y-1)||u.push("scale("+S.point(c)+")"),r.transform=u.join(" ")}else r.transform="matrix("+t.getValues().join(",")+")"}return r}function s(e,i){for(var s=r(e._matrix),a=e._children,o=n("g",s),h=0,u=a.length;u>h;h++){var l=a[h],c=b(l,i);if(c)if(l.isClipMask()){var d=n("clipPath");d.appendChild(c),w(l,d,"clip"),t(o,{"clip-path":"url(#"+d.id+")"})}else o.appendChild(c)}return o}function h(t){var e=r(t._matrix,!0),i=t.getSize();return e.x-=i.width/2,e.y-=i.height/2,e.width=i.width,e.height=i.height,e.href=t.toDataURL(),n("image",e)}function u(t,e){if(e.matchShapes){var s=t.toShape(!1);if(s)return c(s,e)}var a,o=t._segments,h=r(t._matrix);if(0===o.length)return null;if(t.isPolygon())if(o.length>=3){a=t._closed?"polygon":"polyline";var u=[];for(i=0,l=o.length;l>i;i++)u.push(S.point(o[i]._point));h.points=u.join(" ")}else{a="line";var d=o[0]._point,f=o[o.length-1]._point;h.set({x1:d.x,y1:d.y,x2:f.x,y2:f.y})}else a="path",h.d=t.getPathData(null,e.precision);return n(a,h)}function c(t){var e=t._type,i=t._radius,s=r(t._matrix,!0,"rectangle"!==e);if("rectangle"===e){e="rect";var a=t._size,o=a.width,h=a.height;s.x-=o/2,s.y-=h/2,s.width=o,s.height=h,i.isZero()&&(i=null)}return i&&("circle"===e?s.r=i:(s.rx=i.width,s.ry=i.height)),n(e,s)}function d(t,e){var i=r(t._matrix),s=t.getPathData(null,e.precision);return s&&(i.d=s),n("path",i)}function f(t,e){var i=r(t._matrix,!0),s=t.getSymbol(),a=m(s,"symbol"),o=s.getDefinition(),h=o.getBounds();return a||(a=n("symbol",{viewBox:S.rectangle(h)}),a.appendChild(b(o,e)),w(s,a,"symbol")),i.href="#"+a.id,i.x+=h.x,i.y+=h.y,i.width=S.number(h.width),i.height=S.number(h.height),n("use",i)}function _(t){var e=m(t,"color");if(!e){var i,r=t.getGradient(),s=r._radial,a=t.getOrigin().transform(),o=t.getDestination().transform();if(s){i={cx:a.x,cy:a.y,r:a.getDistance(o)};var h=t.getHighlight();h&&(h=h.transform(),i.fx=h.x,i.fy=h.y)}else i={x1:a.x,y1:a.y,x2:o.x,y2:o.y};i.gradientUnits="userSpaceOnUse",e=n((s?"radial":"linear")+"Gradient",i);for(var u=r._stops,l=0,c=u.length;c>l;l++){var d=u[l],f=d._color,_=f.getAlpha();i={offset:d._rampPoint,"stop-color":f.toCSS(!0)},1>_&&(i["stop-opacity"]=_),e.appendChild(n("stop",i))}w(t,e,"color")}return"url(#"+e.id+")"}function g(t){var e=n("text",r(t._matrix,!0));return e.textContent=t._content,e}function p(n,i,r){var s={},a=!r&&n.getParent();return null!=n._name&&(s.id=n._name),e.each(te,function(t){var i=t.get,r=t.type,o=n[i]();if(t.exportFilter?t.exportFilter(n,o):!a||!e.equals(a[i](),o)){if("color"===r&&null!=o){var h=o.getAlpha();1>h&&(s[t.attribute+"-opacity"]=h)}s[t.attribute]=null==o?"none":"number"===r?S.number(o):"color"===r?o.gradient?_(o,n):o.toCSS(!0):"array"===r?o.join(","):"lookup"===r?t.toSVG[o]:o}}),1===s.opacity&&delete s.opacity,n._visible||(s.visibility="hidden"),t(i,s)}function m(t,e){return P||(P={ids:{},svgs:{}}),t&&P.svgs[e+"-"+t._id]}function w(t,e,n){P||m();var i=P.ids[n]=(P.ids[n]||0)+1;e.id=n+"-"+i,P.svgs[n+"-"+t._id]=e}function x(t,e){var i=t,r=null;if(P){i="svg"===t.nodeName.toLowerCase()&&t;for(var s in P.svgs)r||(i||(i=n("svg"),i.appendChild(t)),r=i.insertBefore(n("defs"),i.firstChild)),r.appendChild(P.svgs[s]);P=null}return e.asString?(new XMLSerializer).serializeToString(i):i}function b(t,e,n){var i=k[t._class],r=i&&i(t,e);if(r){var s=e.onExport;s&&(r=s(t,r,e)||r);var a=JSON.stringify(t._data);a&&"{}"!==a&&r.setAttribute("data-paper-data",a)}return r&&p(t,r,n)}function C(t){return t||(t={}),S=new a(t.precision),t}var S,P,k={Group:s,Layer:s,Raster:h,Path:u,Shape:c,CompoundPath:d,PlacedSymbol:f,PointText:g};y.inject({exportSVG:function(t){return t=C(t),x(b(this,t,!0),t)}}),v.inject({exportSVG:function(t){t=C(t);var e=this.layers,i=this.getView(),s=i.getViewSize(),a=n("svg",{x:0,y:0,width:s.width,height:s.height,version:"1.1",xmlns:"http://www.w3.org/2000/svg","xmlns:xlink":"http://www.w3.org/1999/xlink"}),o=a,h=i._matrix;h.isIdentity()||(o=a.appendChild(n("g",r(h))));for(var u=0,l=e.length;l>u;u++)o.appendChild(b(e[u],t,!0));return x(a,t)}})},new function(){function n(t,e,n,i){var r=ee[e],s=r?t.getAttributeNS(r,e):t.getAttribute(e);return"null"===s&&(s=null),null==s?i?null:n?"":0:n?s:parseFloat(s)}function i(t,e,i,r){return e=n(t,e,!1,r),i=n(t,i,!1,r),!r||null!=e&&null!=i?new h(e,i):null}function r(t,e,i,r){return e=n(t,e,!1,r),i=n(t,i,!1,r),!r||null!=e&&null!=i?new c(e,i):null}function s(t,e,n){return"none"===t?null:"number"===e?parseFloat(t):"array"===e?t?t.split(/[\s,]+/g).map(parseFloat):[]:"color"===e?S(t)||t:"lookup"===e?n[t]:t}function a(t,e,n,i){var r=t.childNodes,s="clippath"===e,a=new w,o=a._project,h=o._currentStyle,u=[];s||(a=x(a,t,i),o._currentStyle=a._style.clone());for(var l=0,c=r.length;c>l;l++){var d,f=r[l];1!==f.nodeType||!(d=P(f,n,!1))||d instanceof m||u.push(d)}return a.addChildren(u),s&&(a=x(a.reduce(),t,i)),o._currentStyle=h,(s||"defs"===e)&&(a.remove(),a=null),a}function o(t,e){for(var n=t.getAttribute("points").match(/[+-]?(?:\d*\.\d+|\d+\.?)(?:[eE][+-]?\d+)?/g),i=[],r=0,s=n.length;s>r;r+=2)i.push(new h(parseFloat(n[r]),parseFloat(n[r+1])));var a=new O(i);return"polygon"===e&&a.closePath(),a}function u(t){var e=t.getAttribute("d"),n={pathData:e};return(e.match(/m/gi)||[]).length>1||/z\S+/i.test(e)?new T(n):new O(n)}function l(t,e){var r,s=(n(t,"href",!0)||"").substring(1),a="radialgradient"===e;if(s)r=I[s].getGradient();else{for(var o=t.childNodes,h=[],u=0,l=o.length;l>u;u++){var c=o[u];1===c.nodeType&&h.push(x(new R,c))}r=new D(h,a)}var d,f,_;return a?(d=i(t,"cx","cy"),f=d.add(n(t,"r"),0),_=i(t,"fx","fy",!0)):(d=i(t,"x1","y1"),f=i(t,"x2","y2")),x(new B(r,d,f,_),t),null}function d(t,e,n,i){for(var r=(i.getAttribute(n)||"").split(/\)\s*/g),s=new g,a=0,o=r.length;o>a;a++){var h=r[a];if(!h)break;for(var u=h.split(/\(\s*/),l=u[0],c=u[1].split(/[\s,]+/g),d=0,f=c.length;f>d;d++)c[d]=parseFloat(c[d]);switch(l){case"matrix":s.concatenate(new g(c[0],c[1],c[2],c[3],c[4],c[5]));break;case"rotate":s.rotate(c[0],c[1],c[2]);break;case"translate":s.translate(c[0],c[1]);break;case"scale":s.scale(c);break;case"skewX":s.skew(c[0],0);break;case"skewY":s.skew(0,c[0])}}t.transform(s)}function _(t,e,n){var i=t["fill-opacity"===n?"getFillColor":"getStrokeColor"]();i&&i.setAlpha(parseFloat(e))}function p(n,i,r){var s=n.attributes[i],a=s&&s.value;if(!a){var o=e.camelize(i);a=n.style[o],a||r.node[o]===r.parent[o]||(a=r.node[o])}return a?"none"===a?null:a:t}function x(n,i,r){var s={node:q.getStyles(i)||{},parent:!r&&q.getStyles(i.parentNode)||{}};return e.each(M,function(r,a){var o=p(i,a,s);o!==t&&(n=e.pick(r(n,o,a,i,s),n))}),n}function S(t){var e=t&&t.match(/\((?:#|)([^)']+)/);return e&&I[e[1]]}function P(t,e,n){function i(t){paper=s;var i=P(t,e,n),r=e.onLoad,a=s.project&&s.getView();r&&r.call(this,i),a.update()}if(!t)return null;e?"function"==typeof e&&(e={onLoad:e}):e={};var r=t,s=paper;if(n)if("string"!=typeof t||/^.*</.test(t)){if("undefined"!=typeof File&&t instanceof File){var a=new FileReader;return a.onload=function(){i(a.result)},a.readAsText(t)}}else{if(r=document.getElementById(t),!r)return Y.request("get",t,i);t=null}if("string"==typeof t&&(r=(new DOMParser).parseFromString(t,"image/svg+xml")),!r.nodeName)throw Error("Unsupported SVG source: "+t);var o,h=r.nodeName.toLowerCase(),u=k[h],l=r.getAttribute&&r.getAttribute("data-paper-data"),c=s.settings,d=c.applyMatrix;if(c.applyMatrix=!1,o=u&&u(r,h,e,n)||null,c.applyMatrix=d,o){"#document"===h||o instanceof w||(o=x(o,r,n));var f=e.onImport;f&&(o=f(r,o,e)||o),e.expandShapes&&o instanceof b&&(o.remove(),o=o.toPath()),l&&(o._data=JSON.parse(l))}return n&&(I={}),o}var k={"#document":function(t,e,n,i){for(var r=t.childNodes,s=0,a=r.length;a>s;s++){var o=r[s];if(1===o.nodeType){var h=o.nextSibling;document.body.appendChild(o);var u=P(o,n,i);return h?t.insertBefore(o,h):t.appendChild(o),u}}},g:a,svg:a,clippath:a,polygon:o,polyline:o,path:u,lineargradient:l,radialgradient:l,image:function(t){var e=new C(n(t,"href",!0));return e.on("load",function(){var e=r(t,"width","height");this.setSize(e);var n=this._matrix._transformPoint(i(t,"x","y").add(e.divide(2)));this.translate(n)}),e},symbol:function(t,e,n,i){return new m(a(t,e,n,i),!0)},defs:a,use:function(t){var e=(n(t,"href",!0)||"").substring(1),r=I[e],s=i(t,"x","y");return r?r instanceof m?r.place(s):r.clone().translate(s):null},circle:function(t){return new b.Circle(i(t,"cx","cy"),n(t,"r"))},ellipse:function(t){return new b.Ellipse({center:i(t,"cx","cy"),radius:r(t,"rx","ry")})},rect:function(t){var e=i(t,"x","y"),n=r(t,"width","height"),s=r(t,"rx","ry");return new b.Rectangle(new f(e,n),s)},line:function(t){return new O.Line(i(t,"x1","y1"),i(t,"x2","y2"))},text:function(t){var e=new j(i(t,"x","y").add(i(t,"dx","dy")));return e.setContent(t.textContent.trim()||""),e}},M=e.each(te,function(t){this[t.attribute]=function(e,n){if(e[t.set](s(n,t.type,t.fromSVG)),"color"===t.type&&e instanceof b){var i=e[t.get]();i&&i.transform((new g).translate(e.getPosition(!0).negate()))}}},{id:function(t,e){I[e]=t,t.setName&&t.setName(e)},"clip-path":function(t,e){var n=S(e);if(n){if(n=n.clone(),n.setClipMask(!0),!(t instanceof w))return new w(n,t);t.insertChild(0,n)}},gradientTransform:d,transform:d,"fill-opacity":_,"stroke-opacity":_,visibility:function(t,e){t.setVisible("visible"===e)},display:function(t,e){t.setVisible(null!==e)},"stop-color":function(t,e){t.setColor&&t.setColor(e)},"stop-opacity":function(t,e){t._color&&t._color.setAlpha(parseFloat(e))},offset:function(t,e){var n=e.match(/(.*)%$/);t.setRampPoint(n?n[1]/100:parseFloat(e))},viewBox:function(t,e,n,i,a){var o=new f(s(e,"array")),h=r(i,"width","height",!0);if(t instanceof w){var u=h?o.getSize().divide(h):1,l=(new g).translate(o.getPoint()).scale(u);t.transform(l.inverted())}else if(t instanceof m){h&&o.setSize(h);var c="visible"!=p(i,"overflow",a),d=t._definition;c&&!o.contains(d.getBounds())&&(c=new b.Rectangle(o).transform(d._matrix),c.setClipMask(!0),d.addChild(c))}}}),I={};y.inject({importSVG:function(t,e){return this.addChild(P(t,e,!0))}}),v.inject({importSVG:function(t,e){return this.activate(),P(t,e,!0)}})},e.exports.PaperScript=function(){function t(t,e,n){var i=g[e];if(t&&t[i]){var r=t[i](n);return"!="===e?!r:r}switch(e){case"+":return t+n;case"-":return t-n;case"*":return t*n;case"/":return t/n;case"%":return t%n;case"==":return t==n;case"!=":return t!=n}}function n(t,e){var n=p[t];if(n&&e&&e[n])return e[n]();switch(t){case"+":return+e;case"-":return-e}}function i(t,e){return _.acorn.parse(t,e)}function s(t,e,n){function r(t){for(var e=0,n=u.length;n>e;e++){var i=u[e];if(i[0]>=t)break;t+=i[1]}return t}function s(e){return t.substring(r(e.range[0]),r(e.range[1]))}function a(e,n){return t.substring(r(e.range[1]),r(n.range[0]))}function o(e,n){for(var i=r(e.range[0]),s=r(e.range[1]),a=0,o=u.length-1;o>=0;o--)if(i>u[o][0]){a=o+1;break}u.splice(a,0,[i,n.length-s+i]),t=t.substring(0,i)+n+t.substring(s)}function h(t,e){if(t){for(var n in t)if("range"!==n&&"loc"!==n){var i=t[n];if(Array.isArray(i))for(var r=0,u=i.length;u>r;r++)h(i[r],t);else i&&"object"==typeof i&&h(i,t)}switch(t.type){case"UnaryExpression":if(t.operator in p&&"Literal"!==t.argument.type){var l=s(t.argument);o(t,'$__("'+t.operator+'", '+l+")")}break;case"BinaryExpression":if(t.operator in g&&"Literal"!==t.left.type){var c=s(t.left),d=s(t.right),f=a(t.left,t.right),_=t.operator;o(t,"__$__("+c+","+f.replace(RegExp("\\"+_),'"'+_+'"')+", "+d+")")}break;case"UpdateExpression":case"AssignmentExpression":var v=e&&e.type;if(!("ForStatement"===v||"BinaryExpression"===v&&/^[=!<>]/.test(e.operator)||"MemberExpression"===v&&e.computed))if("UpdateExpression"===t.type){var l=s(t.argument),m=l+" = __$__("+l+', "'+t.operator[0]+'", 1)';t.prefix||"AssignmentExpression"!==v&&"VariableDeclarator"!==v||(m=l+"; "+m),o(t,m)}else if(/^.=$/.test(t.operator)&&"Literal"!==t.left.type){var c=s(t.left),d=s(t.right);o(t,c+" = __$__("+c+', "'+t.operator[0]+'", '+d+")")}}}}if(!t)return"";n=n||{},e=e||"";var u=[],l=null,c=paper.browser,d=c.versionNumber,f=/\r\n|\n|\r/gm;if(c.chrome&&d>=30||c.webkit&&d>=537.76||c.firefox&&d>=23){var _=0;if(0===window.location.href.indexOf(e)){var v=document.getElementsByTagName("html")[0].innerHTML;_=v.substr(0,v.indexOf(t)+1).match(f).length+1}var m=["AAAA"];m.length=(t.match(f)||[]).length+1+_,l={version:3,file:e,names:[],mappings:m.join(";AACA"),sourceRoot:"",sources:[e]};var y=n.source||!e&&t;y&&(l.sourcesContent=[y])}return h(i(t,{ranges:!0})),l&&(t=Array(_+1).join("\n")+t+"\n//# sourceMappingURL=data:application/json;base64,"+btoa(unescape(encodeURIComponent(JSON.stringify(l))))+"\n//# sourceURL="+(e||"paperscript")),t}function a(i,r,a,o){function u(t,e){for(var n in t)!e&&/^_/.test(n)||!RegExp("([\\b\\s\\W]|^)"+n.replace(/\$/g,"\\$")+"\\b").test(i)||(g.push(n),p.push(t[n]))}paper=r;var l,c=r.getView(),d=/\s+on(?:Key|Mouse)(?:Up|Down|Move|Drag)\b/.test(i)?new J:null,f=d?d._events:[],_=["onFrame","onResize"].concat(f),g=[],p=[];i=s(i,a,o),u({__$__:t,$__:n,paper:r,view:c,tool:d},!0),u(r),_=e.each(_,function(t){RegExp("\\s+"+t+"\\b").test(i)&&(g.push(t),this.push(t+": "+t))},[]).join(", "),_&&(i+="\nreturn { "+_+" };");var v=paper.browser;if(v.chrome||v.firefox){var m=document.createElement("script"),y=document.head;v.firefox&&(i="\n"+i),m.appendChild(document.createTextNode("paper._execute = function("+g+") {"+i+"\n}")),y.appendChild(m),l=paper._execute,delete paper._execute,y.removeChild(m)}else l=Function(g,i);var w=l.apply(r,p)||{};e.each(f,function(t){var e=w[t];e&&(d[t]=e)}),c&&(w.onResize&&c.setOnResize(w.onResize),c.emit("resize",{size:c.size,delta:new h}),w.onFrame&&c.setOnFrame(w.onFrame),c.update())}function o(t){if(/^text\/(?:x-|)paperscript$/.test(t.type)&&"true"!==r.getAttribute(t,"ignore")){var e=r.getAttribute(t,"canvas"),n=document.getElementById(e),i=t.src,s="data-paper-scope";if(!n)throw Error('Unable to find canvas with id "'+e+'"');var o=r.get(n.getAttribute(s))||(new r).setup(n);return n.setAttribute(s,o._id),i?Y.request("get",i,function(t){a(t,o,i)}):a(t.innerHTML,o,t.baseURI),t.setAttribute("data-paper-ignore","true"),o}}function u(){e.each(document.getElementsByTagName("script"),o)}function l(t){return t?o(t):u()}var d,f,_=this;!function(t,e){return"object"==typeof d&&"object"==typeof module?e(d):"function"==typeof f&&f.amd?f(["exports"],e):(e(t.acorn||(t.acorn={})),void 0)}(this,function(t){"use strict";function e(t){ce=t||{};for(var e in ge)Object.prototype.hasOwnProperty.call(ce,e)||(ce[e]=ge[e]);_e=ce.sourceFile||null}function n(t,e){var n=pe(de,t);e+=" ("+n.line+":"+n.column+")";var i=new SyntaxError(e);throw i.pos=t,i.loc=n,i.raisedAt=ve,i}function i(t){function e(t){if(1==t.length)return n+="return str === "+JSON.stringify(t[0])+";";n+="switch(str){";for(var e=0;e<t.length;++e)n+="case "+JSON.stringify(t[e])+":";n+="return true}return false;"}t=t.split(" ");var n="",i=[];t:for(var r=0;r<t.length;++r){for(var s=0;s<i.length;++s)if(i[s][0].length==t[r].length){i[s].push(t[r]);continue t}i.push([t[r]])}if(i.length>3){i.sort(function(t,e){return e.length-t.length}),n+="switch(str.length){";for(var r=0;r<i.length;++r){var a=i[r];n+="case "+a[0].length+":",e(a)}n+="}"}else e(t);return Function("str",n)}function r(){this.line=Pe,this.column=ve-ke}function s(){Pe=1,ve=ke=0,Se=!0,u()}function a(t,e){ye=ve,ce.locations&&(xe=new r),be=t,u(),Ce=e,Se=t.beforeExpr}function o(){var t=ce.onComment&&ce.locations&&new r,e=ve,i=de.indexOf("*/",ve+=2);if(-1===i&&n(ve-2,"Unterminated comment"),ve=i+2,ce.locations){Yn.lastIndex=e;for(var s;(s=Yn.exec(de))&&s.index<ve;)++Pe,ke=s.index+s[0].length}ce.onComment&&ce.onComment(!0,de.slice(e+2,i),e,ve,t,ce.locations&&new r)}function h(){for(var t=ve,e=ce.onComment&&ce.locations&&new r,n=de.charCodeAt(ve+=2);fe>ve&&10!==n&&13!==n&&8232!==n&&8233!==n;)++ve,n=de.charCodeAt(ve);ce.onComment&&ce.onComment(!1,de.slice(t+2,ve),t,ve,e,ce.locations&&new r)}function u(){for(;fe>ve;){var t=de.charCodeAt(ve);if(32===t)++ve;else if(13===t){++ve;var e=de.charCodeAt(ve);10===e&&++ve,ce.locations&&(++Pe,ke=ve)}else if(10===t||8232===t||8233===t)++ve,ce.locations&&(++Pe,ke=ve);else if(t>8&&14>t)++ve;else if(47===t){var e=de.charCodeAt(ve+1);if(42===e)o();else{if(47!==e)break;h()}}else if(160===t)++ve;else{if(!(t>=5760&&Hn.test(String.fromCharCode(t))))break;++ve}}}function l(){var t=de.charCodeAt(ve+1);return t>=48&&57>=t?S(!0):(++ve,a(xn))}function c(){var t=de.charCodeAt(ve+1);return Se?(++ve,x()):61===t?w(Pn,2):w(Cn,1)}function d(){var t=de.charCodeAt(ve+1);return 61===t?w(Pn,2):w(Bn,1)}function f(t){var e=de.charCodeAt(ve+1);return e===t?w(124===t?zn:An,2):61===e?w(Pn,2):w(124===t?On:Ln,1)}function _(){var t=de.charCodeAt(ve+1);return 61===t?w(Pn,2):w(Tn,1)}function g(t){var e=de.charCodeAt(ve+1);return e===t?45==e&&62==de.charCodeAt(ve+2)&&Jn.test(de.slice(Ie,ve))?(ve+=3,h(),u(),y()):w(Mn,2):61===e?w(Pn,2):w(kn,1)}function p(t){var e=de.charCodeAt(ve+1),n=1;return e===t?(n=62===t&&62===de.charCodeAt(ve+2)?3:2,61===de.charCodeAt(ve+n)?w(Pn,n+1):w(jn,n)):33==e&&60==t&&45==de.charCodeAt(ve+2)&&45==de.charCodeAt(ve+3)?(ve+=4,h(),u(),y()):(61===e&&(n=61===de.charCodeAt(ve+2)?3:2),w(Nn,n))}function v(t){var e=de.charCodeAt(ve+1);return 61===e?w(En,61===de.charCodeAt(ve+2)?3:2):w(61===t?Sn:In,1)}function m(t){switch(t){case 46:return l();case 40:return++ve,a(pn);case 41:return++ve,a(vn);case 59:return++ve,a(yn);case 44:return++ve,a(mn);case 91:return++ve,a(dn);case 93:return++ve,a(fn);case 123:return++ve,a(_n);case 125:return++ve,a(gn);case 58:return++ve,a(wn);case 63:return++ve,a(bn);case 48:var e=de.charCodeAt(ve+1);if(120===e||88===e)return C();case 49:case 50:case 51:case 52:case 53:case 54:case 55:case 56:case 57:return S(!1);case 34:case 39:return P(t);case 47:return c(t);case 37:case 42:return d();case 124:case 38:return f(t);case 94:return _();case 43:case 45:return g(t);case 60:case 62:return p(t);case 61:case 33:return v(t);case 126:return w(In,1)}return!1}function y(t){if(t?ve=me+1:me=ve,ce.locations&&(we=new r),t)return x();if(ve>=fe)return a(De);var e=de.charCodeAt(ve);if(Kn(e)||92===e)return I();var i=m(e);if(i===!1){var s=String.fromCharCode(e);if("\\"===s||$n.test(s))return I();n(ve,"Unexpected character '"+s+"'")}return i}function w(t,e){var n=de.slice(ve,ve+e);ve+=e,a(t,n)}function x(){for(var t,e,i="",r=ve;;){ve>=fe&&n(r,"Unterminated regular expression");var s=de.charAt(ve);if(Jn.test(s)&&n(r,"Unterminated regular expression"),t)t=!1;else{if("["===s)e=!0;else if("]"===s&&e)e=!1;else if("/"===s&&!e)break;t="\\"===s}++ve}var i=de.slice(r,ve);++ve;var o=M();return o&&!/^[gmsiy]*$/.test(o)&&n(r,"Invalid regexp flag"),a(Ne,RegExp(i,o))}function b(t,e){for(var n=ve,i=0,r=0,s=null==e?1/0:e;s>r;++r){var a,o=de.charCodeAt(ve);if(a=o>=97?o-97+10:o>=65?o-65+10:o>=48&&57>=o?o-48:1/0,a>=t)break;++ve,i=i*t+a}return ve===n||null!=e&&ve-n!==e?null:i}function C(){ve+=2;var t=b(16);return null==t&&n(me+2,"Expected hexadecimal number"),Kn(de.charCodeAt(ve))&&n(ve,"Identifier directly after number"),a(Ee,t)}function S(t){var e=ve,i=!1,r=48===de.charCodeAt(ve);t||null!==b(10)||n(e,"Invalid number"),46===de.charCodeAt(ve)&&(++ve,b(10),i=!0);var s=de.charCodeAt(ve);(69===s||101===s)&&(s=de.charCodeAt(++ve),(43===s||45===s)&&++ve,null===b(10)&&n(e,"Invalid number"),i=!0),Kn(de.charCodeAt(ve))&&n(ve,"Identifier directly after number");var o,h=de.slice(e,ve);return i?o=parseFloat(h):r&&1!==h.length?/[89]/.test(h)||Te?n(e,"Invalid number"):o=parseInt(h,8):o=parseInt(h,10),a(Ee,o)}function P(t){ve++;for(var e="";;){ve>=fe&&n(me,"Unterminated string constant");var i=de.charCodeAt(ve);if(i===t)return++ve,a(je,e);if(92===i){i=de.charCodeAt(++ve);var r=/^[0-7]+/.exec(de.slice(ve,ve+3));for(r&&(r=r[0]);r&&parseInt(r,8)>255;)r=r.slice(0,r.length-1);if("0"===r&&(r=null),++ve,r)Te&&n(ve-2,"Octal literal in strict mode"),e+=String.fromCharCode(parseInt(r,8)),ve+=r.length-1;else switch(i){case 110:e+="\n";break;case 114:e+="\r";break;case 120:e+=String.fromCharCode(k(2));break;case 117:e+=String.fromCharCode(k(4));break;case 85:e+=String.fromCharCode(k(8));break;case 116:e+="	";break;case 98:e+="\b";break;case 118:e+="";break;case 102:e+="\f";break;case 48:e+="\0";break;case 13:10===de.charCodeAt(ve)&&++ve;case 10:ce.locations&&(ke=ve,++Pe);break;default:e+=String.fromCharCode(i)}}else(13===i||10===i||8232===i||8233===i)&&n(me,"Unterminated string constant"),e+=String.fromCharCode(i),++ve}}function k(t){var e=b(16,t);return null===e&&n(me,"Bad character escape sequence"),e}function M(){Rn=!1;for(var t,e=!0,i=ve;;){var r=de.charCodeAt(ve);if(Qn(r))Rn&&(t+=de.charAt(ve)),++ve;else{if(92!==r)break;Rn||(t=de.slice(i,ve)),Rn=!0,117!=de.charCodeAt(++ve)&&n(ve,"Expecting Unicode escape sequence \\uXXXX"),++ve;var s=k(4),a=String.fromCharCode(s);a||n(ve-1,"Invalid Unicode escape"),(e?Kn(s):Qn(s))||n(ve-4,"Invalid Unicode escape"),t+=a}e=!1}return Rn?t:de.slice(i,ve)}function I(){var t=M(),e=Be;return Rn||(Un(t)?e=cn[t]:(ce.forbidReserved&&(3===ce.ecmaVersion?Fn:qn)(t)||Te&&Vn(t))&&n(me,"The keyword '"+t+"' is reserved")),a(e,t)}function z(){Me=me,Ie=ye,ze=xe,y()}function A(t){if(Te=t,ve=Ie,ce.locations)for(;ke>ve;)ke=de.lastIndexOf("\n",ke-2)+1,--Pe;u(),y()}function O(){this.type=null,this.start=me,this.end=null}function T(){this.start=we,this.end=null,null!==_e&&(this.source=_e)}function L(){var t=new O;return ce.locations&&(t.loc=new T),ce.ranges&&(t.range=[me,0]),t}function E(t){var e=new O;return e.start=t.start,ce.locations&&(e.loc=new T,e.loc.start=t.loc.start),ce.ranges&&(e.range=[t.range[0],0]),e}function N(t,e){return t.type=e,t.end=Ie,ce.locations&&(t.loc.end=ze),ce.ranges&&(t.range[1]=Ie),t}function j(t){return ce.ecmaVersion>=5&&"ExpressionStatement"===t.type&&"Literal"===t.expression.type&&"use strict"===t.expression.value}function B(t){return be===t?(z(),!0):void 0}function D(){return!ce.strictSemicolons&&(be===De||be===gn||Jn.test(de.slice(Ie,me)))}function R(){B(yn)||D()||q()}function F(t){be===t?z():q()}function q(){n(me,"Unexpected token")}function V(t){"Identifier"!==t.type&&"MemberExpression"!==t.type&&n(t.start,"Assigning to rvalue"),Te&&"Identifier"===t.type&&Zn(t.name)&&n(t.start,"Assigning to "+t.name+" in strict mode")}function Z(t){Me=Ie=ve,ce.locations&&(ze=new r),Ae=Te=null,Oe=[],y();var e=t||L(),n=!0;for(t||(e.body=[]);be!==De;){var i=U();e.body.push(i),n&&j(i)&&A(!0),n=!1}return N(e,"Program")}function U(){(be===Cn||be===Pn&&"/="==Ce)&&y(!0);var t=be,e=L();switch(t){case Re:case Ve:z();var i=t===Re;B(yn)||D()?e.label=null:be!==Be?q():(e.label=le(),R());for(var r=0;r<Oe.length;++r){var s=Oe[r];if(null==e.label||s.name===e.label.name){if(null!=s.kind&&(i||"loop"===s.kind))break;if(e.label&&i)break}}return r===Oe.length&&n(e.start,"Unsyntactic "+t.keyword),N(e,i?"BreakStatement":"ContinueStatement");case Ze:return z(),R(),N(e,"DebuggerStatement");case He:return z(),Oe.push(ti),e.body=U(),Oe.pop(),F(nn),e.test=H(),R(),N(e,"DoWhileStatement");case $e:if(z(),Oe.push(ti),F(pn),be===yn)return G(e,null);if(be===en){var a=L();return z(),X(a,!0),N(a,"VariableDeclaration"),1===a.declarations.length&&B(ln)?$(e,a):G(e,a)}var a=J(!1,!0);return B(ln)?(V(a),$(e,a)):G(e,a);case Xe:return z(),he(e,!0);case Je:return z(),e.test=H(),e.consequent=U(),e.alternate=B(We)?U():null,N(e,"IfStatement");case Ye:return Ae||n(me,"'return' outside of function"),z(),B(yn)||D()?e.argument=null:(e.argument=J(),R()),N(e,"ReturnStatement");case Ke:z(),e.discriminant=H(),e.cases=[],F(_n),Oe.push(ei);for(var o,h;be!=gn;)if(be===Fe||be===Ue){var u=be===Fe;o&&N(o,"SwitchCase"),e.cases.push(o=L()),o.consequent=[],z(),u?o.test=J():(h&&n(Me,"Multiple default clauses"),h=!0,o.test=null),F(wn)}else o||q(),o.consequent.push(U());return o&&N(o,"SwitchCase"),z(),Oe.pop(),N(e,"SwitchStatement");case Qe:return z(),Jn.test(de.slice(Ie,me))&&n(Ie,"Illegal newline after throw"),e.argument=J(),R(),N(e,"ThrowStatement");case tn:if(z(),e.block=W(),e.handler=null,be===qe){var l=L();z(),F(pn),l.param=le(),Te&&Zn(l.param.name)&&n(l.param.start,"Binding "+l.param.name+" in strict mode"),F(vn),l.guard=null,l.body=W(),e.handler=N(l,"CatchClause")}return e.guardedHandlers=Le,e.finalizer=B(Ge)?W():null,e.handler||e.finalizer||n(e.start,"Missing catch or finally clause"),N(e,"TryStatement");case en:return z(),X(e),R(),N(e,"VariableDeclaration");case nn:return z(),e.test=H(),Oe.push(ti),e.body=U(),Oe.pop(),N(e,"WhileStatement");case rn:return Te&&n(me,"'with' in strict mode"),z(),e.object=H(),e.body=U(),N(e,"WithStatement");case _n:return W();case yn:return z(),N(e,"EmptyStatement");default:var c=Ce,d=J();if(t===Be&&"Identifier"===d.type&&B(wn)){for(var r=0;r<Oe.length;++r)Oe[r].name===c&&n(d.start,"Label '"+c+"' is already declared");var f=be.isLoop?"loop":be===Ke?"switch":null;return Oe.push({name:c,kind:f}),e.body=U(),Oe.pop(),e.label=d,N(e,"LabeledStatement")}return e.expression=d,R(),N(e,"ExpressionStatement")}}function H(){F(pn);var t=J();return F(vn),t}function W(t){var e,n=L(),i=!0,r=!1;for(n.body=[],F(_n);!B(gn);){var s=U();n.body.push(s),i&&t&&j(s)&&(e=r,A(r=!0)),i=!1}return r&&!e&&A(!1),N(n,"BlockStatement")}function G(t,e){return t.init=e,F(yn),t.test=be===yn?null:J(),F(yn),t.update=be===vn?null:J(),F(vn),t.body=U(),Oe.pop(),N(t,"ForStatement")}function $(t,e){return t.left=e,t.right=J(),F(vn),t.body=U(),Oe.pop(),N(t,"ForInStatement")}function X(t,e){for(t.declarations=[],t.kind="var";;){var i=L();if(i.id=le(),Te&&Zn(i.id.name)&&n(i.id.start,"Binding "+i.id.name+" in strict mode"),i.init=B(Sn)?J(!0,e):null,t.declarations.push(N(i,"VariableDeclarator")),!B(mn))break
}return t}function J(t,e){var n=Y(e);if(!t&&be===mn){var i=E(n);for(i.expressions=[n];B(mn);)i.expressions.push(Y(e));return N(i,"SequenceExpression")}return n}function Y(t){var e=K(t);if(be.isAssign){var n=E(e);return n.operator=Ce,n.left=e,z(),n.right=Y(t),V(e),N(n,"AssignmentExpression")}return e}function K(t){var e=Q(t);if(B(bn)){var n=E(e);return n.test=e,n.consequent=J(!0),F(wn),n.alternate=J(!0,t),N(n,"ConditionalExpression")}return e}function Q(t){return te(ee(),-1,t)}function te(t,e,n){var i=be.binop;if(null!=i&&(!n||be!==ln)&&i>e){var r=E(t);r.left=t,r.operator=Ce,z(),r.right=te(ee(),i,n);var s=N(r,/&&|\|\|/.test(r.operator)?"LogicalExpression":"BinaryExpression");return te(s,e,n)}return t}function ee(){if(be.prefix){var t=L(),e=be.isUpdate;return t.operator=Ce,t.prefix=!0,Se=!0,z(),t.argument=ee(),e?V(t.argument):Te&&"delete"===t.operator&&"Identifier"===t.argument.type&&n(t.start,"Deleting local variable in strict mode"),N(t,e?"UpdateExpression":"UnaryExpression")}for(var i=ne();be.postfix&&!D();){var t=E(i);t.operator=Ce,t.prefix=!1,t.argument=i,V(i),z(),i=N(t,"UpdateExpression")}return i}function ne(){return ie(re())}function ie(t,e){if(B(xn)){var n=E(t);return n.object=t,n.property=le(!0),n.computed=!1,ie(N(n,"MemberExpression"),e)}if(B(dn)){var n=E(t);return n.object=t,n.property=J(),n.computed=!0,F(fn),ie(N(n,"MemberExpression"),e)}if(!e&&B(pn)){var n=E(t);return n.callee=t,n.arguments=ue(vn,!1),ie(N(n,"CallExpression"),e)}return t}function re(){switch(be){case an:var t=L();return z(),N(t,"ThisExpression");case Be:return le();case Ee:case je:case Ne:var t=L();return t.value=Ce,t.raw=de.slice(me,ye),z(),N(t,"Literal");case on:case hn:case un:var t=L();return t.value=be.atomValue,t.raw=be.keyword,z(),N(t,"Literal");case pn:var e=we,n=me;z();var i=J();return i.start=n,i.end=ye,ce.locations&&(i.loc.start=e,i.loc.end=xe),ce.ranges&&(i.range=[n,ye]),F(vn),i;case dn:var t=L();return z(),t.elements=ue(fn,!0,!0),N(t,"ArrayExpression");case _n:return ae();case Xe:var t=L();return z(),he(t,!1);case sn:return se();default:q()}}function se(){var t=L();return z(),t.callee=ie(re(),!0),t.arguments=B(pn)?ue(vn,!1):Le,N(t,"NewExpression")}function ae(){var t=L(),e=!0,i=!1;for(t.properties=[],z();!B(gn);){if(e)e=!1;else if(F(mn),ce.allowTrailingCommas&&B(gn))break;var r,s={key:oe()},a=!1;if(B(wn)?(s.value=J(!0),r=s.kind="init"):ce.ecmaVersion>=5&&"Identifier"===s.key.type&&("get"===s.key.name||"set"===s.key.name)?(a=i=!0,r=s.kind=s.key.name,s.key=oe(),be!==pn&&q(),s.value=he(L(),!1)):q(),"Identifier"===s.key.type&&(Te||i))for(var o=0;o<t.properties.length;++o){var h=t.properties[o];if(h.key.name===s.key.name){var u=r==h.kind||a&&"init"===h.kind||"init"===r&&("get"===h.kind||"set"===h.kind);u&&!Te&&"init"===r&&"init"===h.kind&&(u=!1),u&&n(s.key.start,"Redefinition of property")}}t.properties.push(s)}return N(t,"ObjectExpression")}function oe(){return be===Ee||be===je?re():le(!0)}function he(t,e){be===Be?t.id=le():e?q():t.id=null,t.params=[];var i=!0;for(F(pn);!B(vn);)i?i=!1:F(mn),t.params.push(le());var r=Ae,s=Oe;if(Ae=!0,Oe=[],t.body=W(!0),Ae=r,Oe=s,Te||t.body.body.length&&j(t.body.body[0]))for(var a=t.id?-1:0;a<t.params.length;++a){var o=0>a?t.id:t.params[a];if((Vn(o.name)||Zn(o.name))&&n(o.start,"Defining '"+o.name+"' in strict mode"),a>=0)for(var h=0;a>h;++h)o.name===t.params[h].name&&n(o.start,"Argument name clash in strict mode")}return N(t,e?"FunctionDeclaration":"FunctionExpression")}function ue(t,e,n){for(var i=[],r=!0;!B(t);){if(r)r=!1;else if(F(mn),e&&ce.allowTrailingCommas&&B(t))break;n&&be===mn?i.push(null):i.push(J(!0))}return i}function le(t){var e=L();return e.name=be===Be?Ce:t&&!ce.forbidReserved&&be.keyword||q(),Se=!1,z(),N(e,"Identifier")}t.version="0.4.0";var ce,de,fe,_e;t.parse=function(t,n){return de=t+"",fe=de.length,e(n),s(),Z(ce.program)};var ge=t.defaultOptions={ecmaVersion:5,strictSemicolons:!1,allowTrailingCommas:!0,forbidReserved:!1,locations:!1,onComment:null,ranges:!1,program:null,sourceFile:null},pe=t.getLineInfo=function(t,e){for(var n=1,i=0;;){Yn.lastIndex=i;var r=Yn.exec(t);if(!(r&&r.index<e))break;++n,i=r.index+r[0].length}return{line:n,column:e-i}};t.tokenize=function(t,n){function i(t){return y(t),r.start=me,r.end=ye,r.startLoc=we,r.endLoc=xe,r.type=be,r.value=Ce,r}de=t+"",fe=de.length,e(n),s();var r={};return i.jumpTo=function(t,e){if(ve=t,ce.locations){Pe=1,ke=Yn.lastIndex=0;for(var n;(n=Yn.exec(de))&&n.index<t;)++Pe,ke=n.index+n[0].length}Se=e,u()},i};var ve,me,ye,we,xe,be,Ce,Se,Pe,ke,Me,Ie,ze,Ae,Oe,Te,Le=[],Ee={type:"num"},Ne={type:"regexp"},je={type:"string"},Be={type:"name"},De={type:"eof"},Re={keyword:"break"},Fe={keyword:"case",beforeExpr:!0},qe={keyword:"catch"},Ve={keyword:"continue"},Ze={keyword:"debugger"},Ue={keyword:"default"},He={keyword:"do",isLoop:!0},We={keyword:"else",beforeExpr:!0},Ge={keyword:"finally"},$e={keyword:"for",isLoop:!0},Xe={keyword:"function"},Je={keyword:"if"},Ye={keyword:"return",beforeExpr:!0},Ke={keyword:"switch"},Qe={keyword:"throw",beforeExpr:!0},tn={keyword:"try"},en={keyword:"var"},nn={keyword:"while",isLoop:!0},rn={keyword:"with"},sn={keyword:"new",beforeExpr:!0},an={keyword:"this"},on={keyword:"null",atomValue:null},hn={keyword:"true",atomValue:!0},un={keyword:"false",atomValue:!1},ln={keyword:"in",binop:7,beforeExpr:!0},cn={"break":Re,"case":Fe,"catch":qe,"continue":Ve,"debugger":Ze,"default":Ue,"do":He,"else":We,"finally":Ge,"for":$e,"function":Xe,"if":Je,"return":Ye,"switch":Ke,"throw":Qe,"try":tn,"var":en,"while":nn,"with":rn,"null":on,"true":hn,"false":un,"new":sn,"in":ln,"instanceof":{keyword:"instanceof",binop:7,beforeExpr:!0},"this":an,"typeof":{keyword:"typeof",prefix:!0,beforeExpr:!0},"void":{keyword:"void",prefix:!0,beforeExpr:!0},"delete":{keyword:"delete",prefix:!0,beforeExpr:!0}},dn={type:"[",beforeExpr:!0},fn={type:"]"},_n={type:"{",beforeExpr:!0},gn={type:"}"},pn={type:"(",beforeExpr:!0},vn={type:")"},mn={type:",",beforeExpr:!0},yn={type:";",beforeExpr:!0},wn={type:":",beforeExpr:!0},xn={type:"."},bn={type:"?",beforeExpr:!0},Cn={binop:10,beforeExpr:!0},Sn={isAssign:!0,beforeExpr:!0},Pn={isAssign:!0,beforeExpr:!0},kn={binop:9,prefix:!0,beforeExpr:!0},Mn={postfix:!0,prefix:!0,isUpdate:!0},In={prefix:!0,beforeExpr:!0},zn={binop:1,beforeExpr:!0},An={binop:2,beforeExpr:!0},On={binop:3,beforeExpr:!0},Tn={binop:4,beforeExpr:!0},Ln={binop:5,beforeExpr:!0},En={binop:6,beforeExpr:!0},Nn={binop:7,beforeExpr:!0},jn={binop:8,beforeExpr:!0},Bn={binop:10,beforeExpr:!0};t.tokTypes={bracketL:dn,bracketR:fn,braceL:_n,braceR:gn,parenL:pn,parenR:vn,comma:mn,semi:yn,colon:wn,dot:xn,question:bn,slash:Cn,eq:Sn,name:Be,eof:De,num:Ee,regexp:Ne,string:je};for(var Dn in cn)t.tokTypes["_"+Dn]=cn[Dn];var Rn,Fn=i("abstract boolean byte char class double enum export extends final float goto implements import int interface long native package private protected public short static super synchronized throws transient volatile"),qn=i("class enum extends super const export import"),Vn=i("implements interface let package private protected public static yield"),Zn=i("eval arguments"),Un=i("break case catch continue debugger default do else finally for function if return switch throw try var while with null true false instanceof typeof void delete new in this"),Hn=/[\u1680\u180e\u2000-\u200a\u202f\u205f\u3000\ufeff]/,Wn="\xaa\xb5\xba\xc0-\xd6\xd8-\xf6\xf8-\u02c1\u02c6-\u02d1\u02e0-\u02e4\u02ec\u02ee\u0370-\u0374\u0376\u0377\u037a-\u037d\u0386\u0388-\u038a\u038c\u038e-\u03a1\u03a3-\u03f5\u03f7-\u0481\u048a-\u0527\u0531-\u0556\u0559\u0561-\u0587\u05d0-\u05ea\u05f0-\u05f2\u0620-\u064a\u066e\u066f\u0671-\u06d3\u06d5\u06e5\u06e6\u06ee\u06ef\u06fa-\u06fc\u06ff\u0710\u0712-\u072f\u074d-\u07a5\u07b1\u07ca-\u07ea\u07f4\u07f5\u07fa\u0800-\u0815\u081a\u0824\u0828\u0840-\u0858\u08a0\u08a2-\u08ac\u0904-\u0939\u093d\u0950\u0958-\u0961\u0971-\u0977\u0979-\u097f\u0985-\u098c\u098f\u0990\u0993-\u09a8\u09aa-\u09b0\u09b2\u09b6-\u09b9\u09bd\u09ce\u09dc\u09dd\u09df-\u09e1\u09f0\u09f1\u0a05-\u0a0a\u0a0f\u0a10\u0a13-\u0a28\u0a2a-\u0a30\u0a32\u0a33\u0a35\u0a36\u0a38\u0a39\u0a59-\u0a5c\u0a5e\u0a72-\u0a74\u0a85-\u0a8d\u0a8f-\u0a91\u0a93-\u0aa8\u0aaa-\u0ab0\u0ab2\u0ab3\u0ab5-\u0ab9\u0abd\u0ad0\u0ae0\u0ae1\u0b05-\u0b0c\u0b0f\u0b10\u0b13-\u0b28\u0b2a-\u0b30\u0b32\u0b33\u0b35-\u0b39\u0b3d\u0b5c\u0b5d\u0b5f-\u0b61\u0b71\u0b83\u0b85-\u0b8a\u0b8e-\u0b90\u0b92-\u0b95\u0b99\u0b9a\u0b9c\u0b9e\u0b9f\u0ba3\u0ba4\u0ba8-\u0baa\u0bae-\u0bb9\u0bd0\u0c05-\u0c0c\u0c0e-\u0c10\u0c12-\u0c28\u0c2a-\u0c33\u0c35-\u0c39\u0c3d\u0c58\u0c59\u0c60\u0c61\u0c85-\u0c8c\u0c8e-\u0c90\u0c92-\u0ca8\u0caa-\u0cb3\u0cb5-\u0cb9\u0cbd\u0cde\u0ce0\u0ce1\u0cf1\u0cf2\u0d05-\u0d0c\u0d0e-\u0d10\u0d12-\u0d3a\u0d3d\u0d4e\u0d60\u0d61\u0d7a-\u0d7f\u0d85-\u0d96\u0d9a-\u0db1\u0db3-\u0dbb\u0dbd\u0dc0-\u0dc6\u0e01-\u0e30\u0e32\u0e33\u0e40-\u0e46\u0e81\u0e82\u0e84\u0e87\u0e88\u0e8a\u0e8d\u0e94-\u0e97\u0e99-\u0e9f\u0ea1-\u0ea3\u0ea5\u0ea7\u0eaa\u0eab\u0ead-\u0eb0\u0eb2\u0eb3\u0ebd\u0ec0-\u0ec4\u0ec6\u0edc-\u0edf\u0f00\u0f40-\u0f47\u0f49-\u0f6c\u0f88-\u0f8c\u1000-\u102a\u103f\u1050-\u1055\u105a-\u105d\u1061\u1065\u1066\u106e-\u1070\u1075-\u1081\u108e\u10a0-\u10c5\u10c7\u10cd\u10d0-\u10fa\u10fc-\u1248\u124a-\u124d\u1250-\u1256\u1258\u125a-\u125d\u1260-\u1288\u128a-\u128d\u1290-\u12b0\u12b2-\u12b5\u12b8-\u12be\u12c0\u12c2-\u12c5\u12c8-\u12d6\u12d8-\u1310\u1312-\u1315\u1318-\u135a\u1380-\u138f\u13a0-\u13f4\u1401-\u166c\u166f-\u167f\u1681-\u169a\u16a0-\u16ea\u16ee-\u16f0\u1700-\u170c\u170e-\u1711\u1720-\u1731\u1740-\u1751\u1760-\u176c\u176e-\u1770\u1780-\u17b3\u17d7\u17dc\u1820-\u1877\u1880-\u18a8\u18aa\u18b0-\u18f5\u1900-\u191c\u1950-\u196d\u1970-\u1974\u1980-\u19ab\u19c1-\u19c7\u1a00-\u1a16\u1a20-\u1a54\u1aa7\u1b05-\u1b33\u1b45-\u1b4b\u1b83-\u1ba0\u1bae\u1baf\u1bba-\u1be5\u1c00-\u1c23\u1c4d-\u1c4f\u1c5a-\u1c7d\u1ce9-\u1cec\u1cee-\u1cf1\u1cf5\u1cf6\u1d00-\u1dbf\u1e00-\u1f15\u1f18-\u1f1d\u1f20-\u1f45\u1f48-\u1f4d\u1f50-\u1f57\u1f59\u1f5b\u1f5d\u1f5f-\u1f7d\u1f80-\u1fb4\u1fb6-\u1fbc\u1fbe\u1fc2-\u1fc4\u1fc6-\u1fcc\u1fd0-\u1fd3\u1fd6-\u1fdb\u1fe0-\u1fec\u1ff2-\u1ff4\u1ff6-\u1ffc\u2071\u207f\u2090-\u209c\u2102\u2107\u210a-\u2113\u2115\u2119-\u211d\u2124\u2126\u2128\u212a-\u212d\u212f-\u2139\u213c-\u213f\u2145-\u2149\u214e\u2160-\u2188\u2c00-\u2c2e\u2c30-\u2c5e\u2c60-\u2ce4\u2ceb-\u2cee\u2cf2\u2cf3\u2d00-\u2d25\u2d27\u2d2d\u2d30-\u2d67\u2d6f\u2d80-\u2d96\u2da0-\u2da6\u2da8-\u2dae\u2db0-\u2db6\u2db8-\u2dbe\u2dc0-\u2dc6\u2dc8-\u2dce\u2dd0-\u2dd6\u2dd8-\u2dde\u2e2f\u3005-\u3007\u3021-\u3029\u3031-\u3035\u3038-\u303c\u3041-\u3096\u309d-\u309f\u30a1-\u30fa\u30fc-\u30ff\u3105-\u312d\u3131-\u318e\u31a0-\u31ba\u31f0-\u31ff\u3400-\u4db5\u4e00-\u9fcc\ua000-\ua48c\ua4d0-\ua4fd\ua500-\ua60c\ua610-\ua61f\ua62a\ua62b\ua640-\ua66e\ua67f-\ua697\ua6a0-\ua6ef\ua717-\ua71f\ua722-\ua788\ua78b-\ua78e\ua790-\ua793\ua7a0-\ua7aa\ua7f8-\ua801\ua803-\ua805\ua807-\ua80a\ua80c-\ua822\ua840-\ua873\ua882-\ua8b3\ua8f2-\ua8f7\ua8fb\ua90a-\ua925\ua930-\ua946\ua960-\ua97c\ua984-\ua9b2\ua9cf\uaa00-\uaa28\uaa40-\uaa42\uaa44-\uaa4b\uaa60-\uaa76\uaa7a\uaa80-\uaaaf\uaab1\uaab5\uaab6\uaab9-\uaabd\uaac0\uaac2\uaadb-\uaadd\uaae0-\uaaea\uaaf2-\uaaf4\uab01-\uab06\uab09-\uab0e\uab11-\uab16\uab20-\uab26\uab28-\uab2e\uabc0-\uabe2\uac00-\ud7a3\ud7b0-\ud7c6\ud7cb-\ud7fb\uf900-\ufa6d\ufa70-\ufad9\ufb00-\ufb06\ufb13-\ufb17\ufb1d\ufb1f-\ufb28\ufb2a-\ufb36\ufb38-\ufb3c\ufb3e\ufb40\ufb41\ufb43\ufb44\ufb46-\ufbb1\ufbd3-\ufd3d\ufd50-\ufd8f\ufd92-\ufdc7\ufdf0-\ufdfb\ufe70-\ufe74\ufe76-\ufefc\uff21-\uff3a\uff41-\uff5a\uff66-\uffbe\uffc2-\uffc7\uffca-\uffcf\uffd2-\uffd7\uffda-\uffdc",Gn="\u0300-\u036f\u0483-\u0487\u0591-\u05bd\u05bf\u05c1\u05c2\u05c4\u05c5\u05c7\u0610-\u061a\u0620-\u0649\u0672-\u06d3\u06e7-\u06e8\u06fb-\u06fc\u0730-\u074a\u0800-\u0814\u081b-\u0823\u0825-\u0827\u0829-\u082d\u0840-\u0857\u08e4-\u08fe\u0900-\u0903\u093a-\u093c\u093e-\u094f\u0951-\u0957\u0962-\u0963\u0966-\u096f\u0981-\u0983\u09bc\u09be-\u09c4\u09c7\u09c8\u09d7\u09df-\u09e0\u0a01-\u0a03\u0a3c\u0a3e-\u0a42\u0a47\u0a48\u0a4b-\u0a4d\u0a51\u0a66-\u0a71\u0a75\u0a81-\u0a83\u0abc\u0abe-\u0ac5\u0ac7-\u0ac9\u0acb-\u0acd\u0ae2-\u0ae3\u0ae6-\u0aef\u0b01-\u0b03\u0b3c\u0b3e-\u0b44\u0b47\u0b48\u0b4b-\u0b4d\u0b56\u0b57\u0b5f-\u0b60\u0b66-\u0b6f\u0b82\u0bbe-\u0bc2\u0bc6-\u0bc8\u0bca-\u0bcd\u0bd7\u0be6-\u0bef\u0c01-\u0c03\u0c46-\u0c48\u0c4a-\u0c4d\u0c55\u0c56\u0c62-\u0c63\u0c66-\u0c6f\u0c82\u0c83\u0cbc\u0cbe-\u0cc4\u0cc6-\u0cc8\u0cca-\u0ccd\u0cd5\u0cd6\u0ce2-\u0ce3\u0ce6-\u0cef\u0d02\u0d03\u0d46-\u0d48\u0d57\u0d62-\u0d63\u0d66-\u0d6f\u0d82\u0d83\u0dca\u0dcf-\u0dd4\u0dd6\u0dd8-\u0ddf\u0df2\u0df3\u0e34-\u0e3a\u0e40-\u0e45\u0e50-\u0e59\u0eb4-\u0eb9\u0ec8-\u0ecd\u0ed0-\u0ed9\u0f18\u0f19\u0f20-\u0f29\u0f35\u0f37\u0f39\u0f41-\u0f47\u0f71-\u0f84\u0f86-\u0f87\u0f8d-\u0f97\u0f99-\u0fbc\u0fc6\u1000-\u1029\u1040-\u1049\u1067-\u106d\u1071-\u1074\u1082-\u108d\u108f-\u109d\u135d-\u135f\u170e-\u1710\u1720-\u1730\u1740-\u1750\u1772\u1773\u1780-\u17b2\u17dd\u17e0-\u17e9\u180b-\u180d\u1810-\u1819\u1920-\u192b\u1930-\u193b\u1951-\u196d\u19b0-\u19c0\u19c8-\u19c9\u19d0-\u19d9\u1a00-\u1a15\u1a20-\u1a53\u1a60-\u1a7c\u1a7f-\u1a89\u1a90-\u1a99\u1b46-\u1b4b\u1b50-\u1b59\u1b6b-\u1b73\u1bb0-\u1bb9\u1be6-\u1bf3\u1c00-\u1c22\u1c40-\u1c49\u1c5b-\u1c7d\u1cd0-\u1cd2\u1d00-\u1dbe\u1e01-\u1f15\u200c\u200d\u203f\u2040\u2054\u20d0-\u20dc\u20e1\u20e5-\u20f0\u2d81-\u2d96\u2de0-\u2dff\u3021-\u3028\u3099\u309a\ua640-\ua66d\ua674-\ua67d\ua69f\ua6f0-\ua6f1\ua7f8-\ua800\ua806\ua80b\ua823-\ua827\ua880-\ua881\ua8b4-\ua8c4\ua8d0-\ua8d9\ua8f3-\ua8f7\ua900-\ua909\ua926-\ua92d\ua930-\ua945\ua980-\ua983\ua9b3-\ua9c0\uaa00-\uaa27\uaa40-\uaa41\uaa4c-\uaa4d\uaa50-\uaa59\uaa7b\uaae0-\uaae9\uaaf2-\uaaf3\uabc0-\uabe1\uabec\uabed\uabf0-\uabf9\ufb20-\ufb28\ufe00-\ufe0f\ufe20-\ufe26\ufe33\ufe34\ufe4d-\ufe4f\uff10-\uff19\uff3f",$n=RegExp("["+Wn+"]"),Xn=RegExp("["+Wn+Gn+"]"),Jn=/[\n\r\u2028\u2029]/,Yn=/\r\n|[\n\r\u2028\u2029]/g,Kn=t.isIdentifierStart=function(t){return 65>t?36===t:91>t?!0:97>t?95===t:123>t?!0:t>=170&&$n.test(String.fromCharCode(t))},Qn=t.isIdentifierChar=function(t){return 48>t?36===t:58>t?!0:65>t?!1:91>t?!0:97>t?95===t:123>t?!0:t>=170&&Xn.test(String.fromCharCode(t))},ti={kind:"loop"},ei={kind:"switch"}});var g={"+":"__add","-":"__subtract","*":"__multiply","/":"__divide","%":"__modulo","==":"equals","!=":"equals"},p={"-":"__negate","+":null},v=e.each(["add","subtract","multiply","divide","modulo","negate"],function(t){this["__"+t]="#"+t},{});return h.inject(v),c.inject(v),B.inject(v),"complete"===document.readyState?setTimeout(u):V.add(window,{load:u}),{compile:s,execute:a,load:l,parse:i}}.call(this),paper=new(r.inject(e.exports,{enumerable:!0,Base:e,Numerical:o,Key:G})),"function"==typeof define&&define.amd?define("paper",paper):"object"==typeof module&&module&&(module.exports=paper),paper};
//     Underscore.js 1.7.0
//     http://underscorejs.org
//     (c) 2009-2014 Jeremy Ashkenas, DocumentCloud and Investigative Reporters & Editors
//     Underscore may be freely distributed under the MIT license.
(function(){var n=this,t=n._,r=Array.prototype,e=Object.prototype,u=Function.prototype,i=r.push,a=r.slice,o=r.concat,l=e.toString,c=e.hasOwnProperty,f=Array.isArray,s=Object.keys,p=u.bind,h=function(n){return n instanceof h?n:this instanceof h?void(this._wrapped=n):new h(n)};"undefined"!=typeof exports?("undefined"!=typeof module&&module.exports&&(exports=module.exports=h),exports._=h):n._=h,h.VERSION="1.7.0";var g=function(n,t,r){if(t===void 0)return n;switch(null==r?3:r){case 1:return function(r){return n.call(t,r)};case 2:return function(r,e){return n.call(t,r,e)};case 3:return function(r,e,u){return n.call(t,r,e,u)};case 4:return function(r,e,u,i){return n.call(t,r,e,u,i)}}return function(){return n.apply(t,arguments)}};h.iteratee=function(n,t,r){return null==n?h.identity:h.isFunction(n)?g(n,t,r):h.isObject(n)?h.matches(n):h.property(n)},h.each=h.forEach=function(n,t,r){if(null==n)return n;t=g(t,r);var e,u=n.length;if(u===+u)for(e=0;u>e;e++)t(n[e],e,n);else{var i=h.keys(n);for(e=0,u=i.length;u>e;e++)t(n[i[e]],i[e],n)}return n},h.map=h.collect=function(n,t,r){if(null==n)return[];t=h.iteratee(t,r);for(var e,u=n.length!==+n.length&&h.keys(n),i=(u||n).length,a=Array(i),o=0;i>o;o++)e=u?u[o]:o,a[o]=t(n[e],e,n);return a};var v="Reduce of empty array with no initial value";h.reduce=h.foldl=h.inject=function(n,t,r,e){null==n&&(n=[]),t=g(t,e,4);var u,i=n.length!==+n.length&&h.keys(n),a=(i||n).length,o=0;if(arguments.length<3){if(!a)throw new TypeError(v);r=n[i?i[o++]:o++]}for(;a>o;o++)u=i?i[o]:o,r=t(r,n[u],u,n);return r},h.reduceRight=h.foldr=function(n,t,r,e){null==n&&(n=[]),t=g(t,e,4);var u,i=n.length!==+n.length&&h.keys(n),a=(i||n).length;if(arguments.length<3){if(!a)throw new TypeError(v);r=n[i?i[--a]:--a]}for(;a--;)u=i?i[a]:a,r=t(r,n[u],u,n);return r},h.find=h.detect=function(n,t,r){var e;return t=h.iteratee(t,r),h.some(n,function(n,r,u){return t(n,r,u)?(e=n,!0):void 0}),e},h.filter=h.select=function(n,t,r){var e=[];return null==n?e:(t=h.iteratee(t,r),h.each(n,function(n,r,u){t(n,r,u)&&e.push(n)}),e)},h.reject=function(n,t,r){return h.filter(n,h.negate(h.iteratee(t)),r)},h.every=h.all=function(n,t,r){if(null==n)return!0;t=h.iteratee(t,r);var e,u,i=n.length!==+n.length&&h.keys(n),a=(i||n).length;for(e=0;a>e;e++)if(u=i?i[e]:e,!t(n[u],u,n))return!1;return!0},h.some=h.any=function(n,t,r){if(null==n)return!1;t=h.iteratee(t,r);var e,u,i=n.length!==+n.length&&h.keys(n),a=(i||n).length;for(e=0;a>e;e++)if(u=i?i[e]:e,t(n[u],u,n))return!0;return!1},h.contains=h.include=function(n,t){return null==n?!1:(n.length!==+n.length&&(n=h.values(n)),h.indexOf(n,t)>=0)},h.invoke=function(n,t){var r=a.call(arguments,2),e=h.isFunction(t);return h.map(n,function(n){return(e?t:n[t]).apply(n,r)})},h.pluck=function(n,t){return h.map(n,h.property(t))},h.where=function(n,t){return h.filter(n,h.matches(t))},h.findWhere=function(n,t){return h.find(n,h.matches(t))},h.max=function(n,t,r){var e,u,i=-1/0,a=-1/0;if(null==t&&null!=n){n=n.length===+n.length?n:h.values(n);for(var o=0,l=n.length;l>o;o++)e=n[o],e>i&&(i=e)}else t=h.iteratee(t,r),h.each(n,function(n,r,e){u=t(n,r,e),(u>a||u===-1/0&&i===-1/0)&&(i=n,a=u)});return i},h.min=function(n,t,r){var e,u,i=1/0,a=1/0;if(null==t&&null!=n){n=n.length===+n.length?n:h.values(n);for(var o=0,l=n.length;l>o;o++)e=n[o],i>e&&(i=e)}else t=h.iteratee(t,r),h.each(n,function(n,r,e){u=t(n,r,e),(a>u||1/0===u&&1/0===i)&&(i=n,a=u)});return i},h.shuffle=function(n){for(var t,r=n&&n.length===+n.length?n:h.values(n),e=r.length,u=Array(e),i=0;e>i;i++)t=h.random(0,i),t!==i&&(u[i]=u[t]),u[t]=r[i];return u},h.sample=function(n,t,r){return null==t||r?(n.length!==+n.length&&(n=h.values(n)),n[h.random(n.length-1)]):h.shuffle(n).slice(0,Math.max(0,t))},h.sortBy=function(n,t,r){return t=h.iteratee(t,r),h.pluck(h.map(n,function(n,r,e){return{value:n,index:r,criteria:t(n,r,e)}}).sort(function(n,t){var r=n.criteria,e=t.criteria;if(r!==e){if(r>e||r===void 0)return 1;if(e>r||e===void 0)return-1}return n.index-t.index}),"value")};var m=function(n){return function(t,r,e){var u={};return r=h.iteratee(r,e),h.each(t,function(e,i){var a=r(e,i,t);n(u,e,a)}),u}};h.groupBy=m(function(n,t,r){h.has(n,r)?n[r].push(t):n[r]=[t]}),h.indexBy=m(function(n,t,r){n[r]=t}),h.countBy=m(function(n,t,r){h.has(n,r)?n[r]++:n[r]=1}),h.sortedIndex=function(n,t,r,e){r=h.iteratee(r,e,1);for(var u=r(t),i=0,a=n.length;a>i;){var o=i+a>>>1;r(n[o])<u?i=o+1:a=o}return i},h.toArray=function(n){return n?h.isArray(n)?a.call(n):n.length===+n.length?h.map(n,h.identity):h.values(n):[]},h.size=function(n){return null==n?0:n.length===+n.length?n.length:h.keys(n).length},h.partition=function(n,t,r){t=h.iteratee(t,r);var e=[],u=[];return h.each(n,function(n,r,i){(t(n,r,i)?e:u).push(n)}),[e,u]},h.first=h.head=h.take=function(n,t,r){return null==n?void 0:null==t||r?n[0]:0>t?[]:a.call(n,0,t)},h.initial=function(n,t,r){return a.call(n,0,Math.max(0,n.length-(null==t||r?1:t)))},h.last=function(n,t,r){return null==n?void 0:null==t||r?n[n.length-1]:a.call(n,Math.max(n.length-t,0))},h.rest=h.tail=h.drop=function(n,t,r){return a.call(n,null==t||r?1:t)},h.compact=function(n){return h.filter(n,h.identity)};var y=function(n,t,r,e){if(t&&h.every(n,h.isArray))return o.apply(e,n);for(var u=0,a=n.length;a>u;u++){var l=n[u];h.isArray(l)||h.isArguments(l)?t?i.apply(e,l):y(l,t,r,e):r||e.push(l)}return e};h.flatten=function(n,t){return y(n,t,!1,[])},h.without=function(n){return h.difference(n,a.call(arguments,1))},h.uniq=h.unique=function(n,t,r,e){if(null==n)return[];h.isBoolean(t)||(e=r,r=t,t=!1),null!=r&&(r=h.iteratee(r,e));for(var u=[],i=[],a=0,o=n.length;o>a;a++){var l=n[a];if(t)a&&i===l||u.push(l),i=l;else if(r){var c=r(l,a,n);h.indexOf(i,c)<0&&(i.push(c),u.push(l))}else h.indexOf(u,l)<0&&u.push(l)}return u},h.union=function(){return h.uniq(y(arguments,!0,!0,[]))},h.intersection=function(n){if(null==n)return[];for(var t=[],r=arguments.length,e=0,u=n.length;u>e;e++){var i=n[e];if(!h.contains(t,i)){for(var a=1;r>a&&h.contains(arguments[a],i);a++);a===r&&t.push(i)}}return t},h.difference=function(n){var t=y(a.call(arguments,1),!0,!0,[]);return h.filter(n,function(n){return!h.contains(t,n)})},h.zip=function(n){if(null==n)return[];for(var t=h.max(arguments,"length").length,r=Array(t),e=0;t>e;e++)r[e]=h.pluck(arguments,e);return r},h.object=function(n,t){if(null==n)return{};for(var r={},e=0,u=n.length;u>e;e++)t?r[n[e]]=t[e]:r[n[e][0]]=n[e][1];return r},h.indexOf=function(n,t,r){if(null==n)return-1;var e=0,u=n.length;if(r){if("number"!=typeof r)return e=h.sortedIndex(n,t),n[e]===t?e:-1;e=0>r?Math.max(0,u+r):r}for(;u>e;e++)if(n[e]===t)return e;return-1},h.lastIndexOf=function(n,t,r){if(null==n)return-1;var e=n.length;for("number"==typeof r&&(e=0>r?e+r+1:Math.min(e,r+1));--e>=0;)if(n[e]===t)return e;return-1},h.range=function(n,t,r){arguments.length<=1&&(t=n||0,n=0),r=r||1;for(var e=Math.max(Math.ceil((t-n)/r),0),u=Array(e),i=0;e>i;i++,n+=r)u[i]=n;return u};var d=function(){};h.bind=function(n,t){var r,e;if(p&&n.bind===p)return p.apply(n,a.call(arguments,1));if(!h.isFunction(n))throw new TypeError("Bind must be called on a function");return r=a.call(arguments,2),e=function(){if(!(this instanceof e))return n.apply(t,r.concat(a.call(arguments)));d.prototype=n.prototype;var u=new d;d.prototype=null;var i=n.apply(u,r.concat(a.call(arguments)));return h.isObject(i)?i:u}},h.partial=function(n){var t=a.call(arguments,1);return function(){for(var r=0,e=t.slice(),u=0,i=e.length;i>u;u++)e[u]===h&&(e[u]=arguments[r++]);for(;r<arguments.length;)e.push(arguments[r++]);return n.apply(this,e)}},h.bindAll=function(n){var t,r,e=arguments.length;if(1>=e)throw new Error("bindAll must be passed function names");for(t=1;e>t;t++)r=arguments[t],n[r]=h.bind(n[r],n);return n},h.memoize=function(n,t){var r=function(e){var u=r.cache,i=t?t.apply(this,arguments):e;return h.has(u,i)||(u[i]=n.apply(this,arguments)),u[i]};return r.cache={},r},h.delay=function(n,t){var r=a.call(arguments,2);return setTimeout(function(){return n.apply(null,r)},t)},h.defer=function(n){return h.delay.apply(h,[n,1].concat(a.call(arguments,1)))},h.throttle=function(n,t,r){var e,u,i,a=null,o=0;r||(r={});var l=function(){o=r.leading===!1?0:h.now(),a=null,i=n.apply(e,u),a||(e=u=null)};return function(){var c=h.now();o||r.leading!==!1||(o=c);var f=t-(c-o);return e=this,u=arguments,0>=f||f>t?(clearTimeout(a),a=null,o=c,i=n.apply(e,u),a||(e=u=null)):a||r.trailing===!1||(a=setTimeout(l,f)),i}},h.debounce=function(n,t,r){var e,u,i,a,o,l=function(){var c=h.now()-a;t>c&&c>0?e=setTimeout(l,t-c):(e=null,r||(o=n.apply(i,u),e||(i=u=null)))};return function(){i=this,u=arguments,a=h.now();var c=r&&!e;return e||(e=setTimeout(l,t)),c&&(o=n.apply(i,u),i=u=null),o}},h.wrap=function(n,t){return h.partial(t,n)},h.negate=function(n){return function(){return!n.apply(this,arguments)}},h.compose=function(){var n=arguments,t=n.length-1;return function(){for(var r=t,e=n[t].apply(this,arguments);r--;)e=n[r].call(this,e);return e}},h.after=function(n,t){return function(){return--n<1?t.apply(this,arguments):void 0}},h.before=function(n,t){var r;return function(){return--n>0?r=t.apply(this,arguments):t=null,r}},h.once=h.partial(h.before,2),h.keys=function(n){if(!h.isObject(n))return[];if(s)return s(n);var t=[];for(var r in n)h.has(n,r)&&t.push(r);return t},h.values=function(n){for(var t=h.keys(n),r=t.length,e=Array(r),u=0;r>u;u++)e[u]=n[t[u]];return e},h.pairs=function(n){for(var t=h.keys(n),r=t.length,e=Array(r),u=0;r>u;u++)e[u]=[t[u],n[t[u]]];return e},h.invert=function(n){for(var t={},r=h.keys(n),e=0,u=r.length;u>e;e++)t[n[r[e]]]=r[e];return t},h.functions=h.methods=function(n){var t=[];for(var r in n)h.isFunction(n[r])&&t.push(r);return t.sort()},h.extend=function(n){if(!h.isObject(n))return n;for(var t,r,e=1,u=arguments.length;u>e;e++){t=arguments[e];for(r in t)c.call(t,r)&&(n[r]=t[r])}return n},h.pick=function(n,t,r){var e,u={};if(null==n)return u;if(h.isFunction(t)){t=g(t,r);for(e in n){var i=n[e];t(i,e,n)&&(u[e]=i)}}else{var l=o.apply([],a.call(arguments,1));n=new Object(n);for(var c=0,f=l.length;f>c;c++)e=l[c],e in n&&(u[e]=n[e])}return u},h.omit=function(n,t,r){if(h.isFunction(t))t=h.negate(t);else{var e=h.map(o.apply([],a.call(arguments,1)),String);t=function(n,t){return!h.contains(e,t)}}return h.pick(n,t,r)},h.defaults=function(n){if(!h.isObject(n))return n;for(var t=1,r=arguments.length;r>t;t++){var e=arguments[t];for(var u in e)n[u]===void 0&&(n[u]=e[u])}return n},h.clone=function(n){return h.isObject(n)?h.isArray(n)?n.slice():h.extend({},n):n},h.tap=function(n,t){return t(n),n};var b=function(n,t,r,e){if(n===t)return 0!==n||1/n===1/t;if(null==n||null==t)return n===t;n instanceof h&&(n=n._wrapped),t instanceof h&&(t=t._wrapped);var u=l.call(n);if(u!==l.call(t))return!1;switch(u){case"[object RegExp]":case"[object String]":return""+n==""+t;case"[object Number]":return+n!==+n?+t!==+t:0===+n?1/+n===1/t:+n===+t;case"[object Date]":case"[object Boolean]":return+n===+t}if("object"!=typeof n||"object"!=typeof t)return!1;for(var i=r.length;i--;)if(r[i]===n)return e[i]===t;var a=n.constructor,o=t.constructor;if(a!==o&&"constructor"in n&&"constructor"in t&&!(h.isFunction(a)&&a instanceof a&&h.isFunction(o)&&o instanceof o))return!1;r.push(n),e.push(t);var c,f;if("[object Array]"===u){if(c=n.length,f=c===t.length)for(;c--&&(f=b(n[c],t[c],r,e)););}else{var s,p=h.keys(n);if(c=p.length,f=h.keys(t).length===c)for(;c--&&(s=p[c],f=h.has(t,s)&&b(n[s],t[s],r,e)););}return r.pop(),e.pop(),f};h.isEqual=function(n,t){return b(n,t,[],[])},h.isEmpty=function(n){if(null==n)return!0;if(h.isArray(n)||h.isString(n)||h.isArguments(n))return 0===n.length;for(var t in n)if(h.has(n,t))return!1;return!0},h.isElement=function(n){return!(!n||1!==n.nodeType)},h.isArray=f||function(n){return"[object Array]"===l.call(n)},h.isObject=function(n){var t=typeof n;return"function"===t||"object"===t&&!!n},h.each(["Arguments","Function","String","Number","Date","RegExp"],function(n){h["is"+n]=function(t){return l.call(t)==="[object "+n+"]"}}),h.isArguments(arguments)||(h.isArguments=function(n){return h.has(n,"callee")}),"function"!=typeof/./&&(h.isFunction=function(n){return"function"==typeof n||!1}),h.isFinite=function(n){return isFinite(n)&&!isNaN(parseFloat(n))},h.isNaN=function(n){return h.isNumber(n)&&n!==+n},h.isBoolean=function(n){return n===!0||n===!1||"[object Boolean]"===l.call(n)},h.isNull=function(n){return null===n},h.isUndefined=function(n){return n===void 0},h.has=function(n,t){return null!=n&&c.call(n,t)},h.noConflict=function(){return n._=t,this},h.identity=function(n){return n},h.constant=function(n){return function(){return n}},h.noop=function(){},h.property=function(n){return function(t){return t[n]}},h.matches=function(n){var t=h.pairs(n),r=t.length;return function(n){if(null==n)return!r;n=new Object(n);for(var e=0;r>e;e++){var u=t[e],i=u[0];if(u[1]!==n[i]||!(i in n))return!1}return!0}},h.times=function(n,t,r){var e=Array(Math.max(0,n));t=g(t,r,1);for(var u=0;n>u;u++)e[u]=t(u);return e},h.random=function(n,t){return null==t&&(t=n,n=0),n+Math.floor(Math.random()*(t-n+1))},h.now=Date.now||function(){return(new Date).getTime()};var _={"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#x27;","`":"&#x60;"},w=h.invert(_),j=function(n){var t=function(t){return n[t]},r="(?:"+h.keys(n).join("|")+")",e=RegExp(r),u=RegExp(r,"g");return function(n){return n=null==n?"":""+n,e.test(n)?n.replace(u,t):n}};h.escape=j(_),h.unescape=j(w),h.result=function(n,t){if(null==n)return void 0;var r=n[t];return h.isFunction(r)?n[t]():r};var x=0;h.uniqueId=function(n){var t=++x+"";return n?n+t:t},h.templateSettings={evaluate:/<%([\s\S]+?)%>/g,interpolate:/<%=([\s\S]+?)%>/g,escape:/<%-([\s\S]+?)%>/g};var A=/(.)^/,k={"'":"'","\\":"\\","\r":"r","\n":"n","\u2028":"u2028","\u2029":"u2029"},O=/\\|'|\r|\n|\u2028|\u2029/g,F=function(n){return"\\"+k[n]};h.template=function(n,t,r){!t&&r&&(t=r),t=h.defaults({},t,h.templateSettings);var e=RegExp([(t.escape||A).source,(t.interpolate||A).source,(t.evaluate||A).source].join("|")+"|$","g"),u=0,i="__p+='";n.replace(e,function(t,r,e,a,o){return i+=n.slice(u,o).replace(O,F),u=o+t.length,r?i+="'+\n((__t=("+r+"))==null?'':_.escape(__t))+\n'":e?i+="'+\n((__t=("+e+"))==null?'':__t)+\n'":a&&(i+="';\n"+a+"\n__p+='"),t}),i+="';\n",t.variable||(i="with(obj||{}){\n"+i+"}\n"),i="var __t,__p='',__j=Array.prototype.join,"+"print=function(){__p+=__j.call(arguments,'');};\n"+i+"return __p;\n";try{var a=new Function(t.variable||"obj","_",i)}catch(o){throw o.source=i,o}var l=function(n){return a.call(this,n,h)},c=t.variable||"obj";return l.source="function("+c+"){\n"+i+"}",l},h.chain=function(n){var t=h(n);return t._chain=!0,t};var E=function(n){return this._chain?h(n).chain():n};h.mixin=function(n){h.each(h.functions(n),function(t){var r=h[t]=n[t];h.prototype[t]=function(){var n=[this._wrapped];return i.apply(n,arguments),E.call(this,r.apply(h,n))}})},h.mixin(h),h.each(["pop","push","reverse","shift","sort","splice","unshift"],function(n){var t=r[n];h.prototype[n]=function(){var r=this._wrapped;return t.apply(r,arguments),"shift"!==n&&"splice"!==n||0!==r.length||delete r[0],E.call(this,r)}}),h.each(["concat","join","slice"],function(n){var t=r[n];h.prototype[n]=function(){return E.call(this,t.apply(this._wrapped,arguments))}}),h.prototype.value=function(){return this._wrapped},"function"==typeof define&&define.amd&&define("underscore",[],function(){return h})}).call(this);
//# sourceMappingURL=underscore-min.map

var cidocs = {};

// set paper scope
paper.install( window );

// array of paper scope
var papers = [];
var COLOR_MOVE_TO	= '#c64b24';
var COLOR_LINE_TO	= '#00FF00';
var COLOR_QUAD_TO	= '#0000FF';
var COLOR_CUBIC_TO	= '#FF00FF';
var COLOR_CLOSE		= '#FF0000';
var COLOR_PATH		= '#000000';
var COLOR_CENTER	= '#f48a00';
var COLOR_INACTIVE	= '#999999';


var SEGMENT_TYPES = Object.freeze( {
	"MOVETO": 0,
	"LINETO": 1,
	"QUADTO": 2,
	"CUBICTO": 3,
	"CLOSE": 4,
	"ARC": 5,
	"ARCTO": 6
} );

var SEGMENT_POINT_COUNTS = [0, 1, 2, 3, 0 ];


//
// from http://codepen.io/MyHatIsAwesome/pen/DaptL
// Function.extend( base )
// sets up inheritance between two constructor functions
// without needing to instantiate the base constructor
//
Function.prototype.extend = function( base )
{
	// Define a surrogate constructor for base
	var Temp = function(){};
	Temp.prototype = base.prototype;

	// instantiate the surrogate
	var t = new Temp();
	t.constructor = this;

	// copy subclass prototype into surrogate object
	for( var key in this.prototype )
		t[key] = this.prototype[key];

	// set subclass prototype as the surrogate object
	this.prototype = t;

	this.prototype.superclass = base.prototype;
};


Function.prototype.addGetter = function( val,fn ){
	this.prototype.__defineGetter__( val,fn );
	return this;
};

Function.prototype.addSetter = function( val,fn ){
	this.prototype.__defineSetter__( val,fn );
	return this;
};


if ( !Number.prototype.getDecimals ) {
	Number.prototype.getDecimals = function() {
		var num = this,
			match = ( '' + num ).match( /(?:\.(\d+))?(?:[eE]([+-]?\d+))?$/ );
		if ( !match )
			return 0;
		return Math.max( 0, ( match[1] ? match[1].length : 0 ) - ( match[2] ? +match[2] : 0 ) );
	};
}

function toCiNum( number, type ) {

	var returnType = type || "string";

	var places = number.getDecimals();
	var output;

	output = number.toString().match( /^-?\d*\.?(?:\d{0,1})?/ ).toString();
	
	// add a decimal point if one isn't present
	if( output.indexOf('.') < 1 ){
		output += '.'
	}
		
	// add a zero if a decimal is there with nothing after it
	if( output.split('.')[1].length < 1 ) {
		output += '0';
	} 
	
	return ( returnType === "string" ) ? output : Number( output );
}

function toCiRadians( radians ) {
	
	var PI2 = ( Math.PI * 2.0 );
	if( radians < 0 ) radians += PI2;
	if( radians >= PI2 ) radians -= PI2;
	radians = Number( toCiNum( radians ) );

	var piRadians = toCiNum( radians / Math.PI );
	var displayRadians;
	
	if( piRadians === 0.0 ) {
		displayRadians = '0.0f';
	} else if( piRadians == 1.0 ) {
		displayRadians = "M_PI";
	} else {
		displayRadians = "M_PI * " + piRadians + 'f';
	}
	return displayRadians;
}

function distance2( p1, p2 ) {
	return p1.getDistance( p2, true );
}


// +———————————————————————————————————————+
//	Segments     
//	directions for how to draw and
//	describe each drawing command
// +———————————————————————————————————————+

cidocs.Segment = function( path2d, type, points ) {

	this.path2d = path2d;
	// this.path2d.points.push( points );
	this.path = this.path2d.path;
	this.type = type;
	this.points = points;
	this.extras = [];

	if( this.path2d.points.length )
		this.pointIndex = this.path2d.points.length-1;
};

cidocs.Segment.prototype = {

	reset: function() {

		_.each( this.extras, function( extra ) {
			extra.remove();
		});
		this.extras = [];
	},

	drawPointText: function( pts ) {

		var self = this;
		_.each( pts, function( pt ){
			var text = new PointText({
				point: [pt.x + 5, pt.y],
				content: '[' + pt.x + ', ' + pt.y + ']',
				fillColor: 'black',
				fontFamily: 'Courier New',
				fontSize: 8
			});
			self.extras.push( text );
		});
	},

	getCode: function() {
		// override
	}
};


cidocs.MoveToSegment = function( path2d, points ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.MOVETO, points );
	this.template = _.template( "mPath.moveTo( vec2( <%- pointX %>f, <%- pointY %>f ) );\n" );
};

cidocs.MoveToSegment.prototype = {

	getCode: function() {
		// console.log("CI NUMBER", toCiNum(0) );
		return this.template( { pointX: toCiNum( this.points[0].position.x ), pointY: toCiNum( this.points[0].position.y ) } );
	}
	
};

cidocs.MoveToSegment.extend( cidocs.Segment );


cidocs.LineToSegment = function( path2d, points ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.LINETO, points );
	this.template = _.template( "mPath.lineTo( vec2( <%= pointX %>f, <%= pointY %>f ) );\n" );
};

cidocs.LineToSegment.prototype = {

	getCode: function() {
		return this.template( { pointX: toCiNum( this.points[0].position.x ), pointY: toCiNum( this.points[0].position.y ) } );		
	}
	
};

cidocs.LineToSegment.extend( cidocs.Segment );


cidocs.QuadToSegment = function( path2d, points ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.QUADTO, points );
	this.template = _.template( "mPath.quadTo( vec2( <%= h1x %>f, <%= h1y %>f ), vec2( <%= p2x %>f, <%= p2y %>f ) );\n" );
};

cidocs.QuadToSegment.prototype = {

	getCode: function() {
		return this.template( { h1x: toCiNum( this.points[0].position.x ), h1y: toCiNum( this.points[0].position.y ), 
								p2x: toCiNum( this.points[1].position.x ), p2y: toCiNum( this.points[1].position.y ) } );
	}
};

cidocs.QuadToSegment.extend( cidocs.Segment );


cidocs.CubicToSegment = function( path2d, points ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.CUBICTO, points );
	this.template = _.template( "mPath.curveTo( vec2( <%= h1x %>f, <%= h1y %>f ), vec2( <%= h2x %>f, <%= h2y %>f ), vec2( <%= p2x %>f, <%= p2y %>f ) );\n" );
};

cidocs.CubicToSegment.prototype = {

	getCode: function() {
		return this.template( { h1x: toCiNum( this.points[0].position.x ), h1y: toCiNum( this.points[0].position.y ), 
								h2x: toCiNum( this.points[1].position.x ), h2y: toCiNum( this.points[1].position.y ), 
								p2x: toCiNum( this.points[2].position.x ), p2y: toCiNum( this.points[2].position.y ) } );
	}
};

cidocs.CubicToSegment.extend( cidocs.Segment );


cidocs.ArcSegment = function( path2d, points, radius, startRadians, endRadians, forward ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.ARC, points );
	this._radius = radius;
	// this.radius = radius;
	this.startRadians = startRadians;
	this.endRadians = endRadians;
	this.forward = forward;
	this.template = _.template( "mPath.arc( vec2( <%= pointX %>f, <%= pointY %>f ), <%= radius %>f, <%= startRadians %>, <%= endRadians %>, <%= forward %> );\n" );
};

cidocs.ArcSegment.prototype = {

	getCode: function() {
		return this.template( { pointX: toCiNum( this.points[0].position.x ), pointY: toCiNum( this.points[0].position.y ),
								radius: toCiNum( this.radius ), startRadians: toCiRadians( this.startRadians ), 
								endRadians: toCiRadians( this.endRadians ), forward: this.forward } );
	}

};

cidocs.ArcSegment.extend( cidocs.Segment );

// getters and setters
cidocs.ArcSegment.addGetter( 'radius', function() { return this._radius; } );
cidocs.ArcSegment.addSetter( 'radius', function( val ){ 
	this._radius = val;
	// console.log( "SET ARC RADIUS, SO DISPATCH AN UPDATE");
	// $(this).trigger( "change_radius" );
});


cidocs.ArcToSegment = function( path2d, points, radius ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.ARCTO, points );
	this.radius = radius;
	this.template = _.template( "mPath.arcTo( vec2( <%= pointX %>f, <%= pointY %>f ), vec2( <%= tanX %>f, <%= tanY %>f ), <%= radius %>f );\n" );
};

cidocs.ArcToSegment.prototype = {

	getCode: function() {
		return this.template( { pointX: toCiNum( this.points[0].position.x ), pointY: toCiNum( this.points[0].position.y ),
								tanX: toCiNum( this.points[1].position.x ), tanY: toCiNum( this.points[1].position.y ),
								radius: toCiNum( this.radius ) } );
	}
};

cidocs.ArcToSegment.extend( cidocs.Segment );


cidocs.CloseSegment = function( path2d ){

	cidocs.Segment.call( this, path2d, SEGMENT_TYPES.CLOSE, [] );
	this.template = _.template( "mPath.close();\n" );
};

cidocs.CloseSegment.prototype = {

	getCode: function() {
		return this.template();
	}
};

cidocs.CloseSegment.extend( cidocs.Segment );


// +———————————————————————————————————————+
//	PathPoint     
//	Serves as the points along a path.
//	Sometimes they are draggable. Sometimes
//	they are just visual
// +———————————————————————————————————————+
cidocs.PathPoint = function( pos, active, color ) {

	var ptRect = new Rectangle( new Point( -2, -2 ), new Size( 4, 4 ) );
	var pt = new Shape.Rectangle( ptRect );
	pt.translate( pos );
	pt.active = ( _.isBoolean( active ) ) ? active : true;
	pt.strokeColor = pt.active ? color || 'blue' : COLOR_INACTIVE;

	return pt;
};

cidocs.StartPoint = function( pos, active ) {

	var star = new Path.Star( new Point( 0, 0 ), 5, 3, 5 );
	star.fillColor = COLOR_MOVE_TO;
	star.strokeColor = COLOR_MOVE_TO;
	star.type = 'star';
	star.rotation = 180;
	var ptStar = new Symbol( star );
	var pt = ptStar.place();
	pt.translate( pos );
	pt.active = ( _.isBoolean( active ) ) ? active : true;

	return pt;
};

cidocs.HandlePoint = function( pos, active ) {

	return new cidocs.PathPoint( pos, active, 'cyan' );
};

cidocs.CenterPoint = function( pos, active ) {

	var circle = new Path.Circle( new Point( 0, 0 ), 3 );
	circle.strokeColor = COLOR_CENTER;
	var circleSymbol = new Symbol( circle );

	var pt = circleSymbol.place();
	pt.translate( pos );
	pt.active = active;

	return pt;
};


// +———————————————————————————————————————+
//	Path2d     
//	wrap a paperjs path to perform more
//	like a cinder path2d
// +———————————————————————————————————————+

cidocs.Path2d = function( paperScope ) {

	this.points		= [];	// to keep track of points
	this.segs		= [];
	this.extras		= [];	// array of points, handles and lines
	this.ps = paperScope;

	this.path = new Path();
	this.path.pivot = new Point();
			
	var star = new Path.Star( new Point( 0, 0 ), 5, 3, 5 );
	star.fillColor = COLOR_MOVE_TO;
	star.strokeColor = COLOR_MOVE_TO;
	star.type = 'star';
	star.rotation = 180;
	this.ptStar = new Symbol( star );

};

cidocs.Path2d.prototype = {
	
	moveTo: function( point ) {

		var pt = new cidocs.StartPoint( point );
		this.points.push( pt );

		var segment = new cidocs.MoveToSegment( this, [_.last( this.points )] );
		this.segs.push( segment );

		this.drawPath();
	},

	lineTo: function( point ) {

		var pt = new cidocs.PathPoint( point );

		this.points.push( pt );

		var segment = new cidocs.LineToSegment( this, [pt] );
		this.segs.push( segment );

		this.drawPath();
	},

	quadTo: function( handlePt, endPt ) {

		var h1 = new cidocs.HandlePoint( handlePt );
		var pt = new cidocs.PathPoint( endPt );
		this.points.push( h1, pt );

		var segment = new cidocs.QuadToSegment( this, [h1, pt] );
		this.segs.push( segment );

		this.drawPath();
	},

	curveTo: function( handlePt1, handlePt2, endPt ) {

		var h1 = new cidocs.HandlePoint( handlePt1 );
		var h2 = new cidocs.HandlePoint( handlePt2 );
		var pt = new cidocs.PathPoint( endPt );
		this.points.push( h1, h2, pt );

		var segment = new cidocs.CubicToSegment( this, [h1, h2, pt] );
		this.segs.push( segment );

		this.drawPath();
	},

	arc: function( center, radius, startRadians, endRadians, frwd ) {

		var pt = new cidocs.CenterPoint( center );

		// get pt at start radians
		var start = new Point( Math.cos( startRadians ), Math.sin( startRadians ) ).multiply( radius ).add( center );
		var startPt = new cidocs.PathPoint( start );

		// get pt at end radians
		var end = new Point( Math.cos( endRadians ), Math.sin( endRadians ) ).multiply( radius ).add( center );
		var endPt = new cidocs.PathPoint( end );

		this.points.push( pt, startPt, endPt );

		var segment = new cidocs.ArcSegment( this, [pt, startPt, endPt], radius, startRadians, endRadians, frwd );
		this.segs.push( segment );
		this.drawPath();
	},

	arcTo: function( targetPt, tangentPt, radius ) {

		var endPt = new cidocs.PathPoint( targetPt );
		var tanPt = new cidocs.PathPoint( tangentPt );
		this.points.push( endPt, tanPt );

		var segment = new cidocs.ArcToSegment( this, [endPt, tanPt], radius );
		this.segs.push( segment );
		this.drawPath();
	},

	reverseArc: function() {

		// for all the segments in the path, reverse the arc directions
		_.each( this.segs, function( segment ) {
			if( segment.type === SEGMENT_TYPES.ARC ) {
				segment.forward = !segment.forward;
			}
		} );
		this.drawPath();
	},

	setArcRadius: function( radius ) {

		// for all the segments in the path, reverse the arc directions
		_.each( this.segs, function( segment ) {

			if( segment.type === SEGMENT_TYPES.ARC ) {

				segment.radius = radius;
				var center = segment.points[0].position;
				var startPt = segment.points[1].position;
				var endPt = segment.points[2].position;
				segment.points[1].position =  new Point( Math.cos( segment.startRadians ), Math.sin( segment.startRadians ) ).multiply( segment.radius ).add( center );
				segment.points[2].position = new Point( Math.cos( segment.endRadians ), Math.sin( segment.endRadians ) ).multiply( segment.radius ).add( center );
			
			} else if ( segment.type === SEGMENT_TYPES.ARCTO ) {
				segment.radius = radius;
			}
		} );
		this.drawPath();
	},

	drawCubicSegment: function( path, segment, options ) {

		var segmentPoints = segment.points;
		var h1 = segmentPoints[0].position;
		var h2 = segmentPoints[1].position;
		var pt = segmentPoints[2].position;
		var drawPointText = _.isBoolean( options.drawPointText ) ? options.drawPointText : true;

		path.cubicCurveTo(
			new Point( h1 ),
			new Point( h2 ),
			new Point( pt )
		);

		if( options.extras ) {

			var l1 = new Path.Line( options.prevPoint, new Point( h1 ) );
			var l2 = new Path.Line( new Point( h2 ), new Point( pt ) );
			var strokeColor = segmentPoints[0].active ? 'cyan' : COLOR_INACTIVE;
			l1.strokeColor = strokeColor;
			l2.strokeColor = strokeColor;
			l1.sendToBack();
			l2.sendToBack();
			this.addExtras( "handle-line", [l1, l2] );
		}

		if( drawPointText ) {
			this.drawPointText.call( this, [h1, h2, pt] );
		}
		
	},

	drawArcSegment: function( path, segment, options ) {

		var center = segment.points[0].position;
		var startPt = segment.points[1].position;
		var endPt = segment.points[2].position;
		var radius = startPt.getDistance( center );
		var startRadians = startPt.subtract( center ).angleInRadians;
		var endRadians = endPt.subtract( center ).angleInRadians;
		var forward = segment.forward;
		var points = this.points;

		if( forward ) {
			while( endRadians < startRadians )
				endRadians += 2 * Math.PI;
		}
		else {
			while( endRadians > startRadians )
				endRadians -= 2 * Math.PI;
		}

		var start =  new Point( Math.cos( startRadians ), Math.sin( startRadians ) ).multiply( radius ).add( center );
		var startMid =  new Point( Math.cos( startRadians ), Math.sin( startRadians ) ).multiply( radius / 2 ).add( center );
		if( points.length === 0 ) {
			path.moveTo( start );
		} else {
			path.lineTo( start );
		}

		if( forward )
			this.arcHelper( path, center, radius, startRadians, endRadians, forward, options );
		else
			this.arcHelper( path, center, radius, endRadians, startRadians, forward, options );

		var radiusLine = new Path.Line( center, start );
		radiusLine.strokeColor = COLOR_CENTER;
		this.addExtras( "radius-line", [radiusLine] );

		this.drawPointText.call( this, [center] );
		this.drawTextAtPoint.call( this, startPt, toCiRadians( startRadians ) );
		this.drawTextAtPoint.call( this, endPt, toCiRadians( endRadians ) );
		this.drawTextAtPoint.call( this, startMid, toCiNum( radius ) + 'f' );
	},

	arcHelper: function( path, center, radius, startRadians, endRadians, forward, options ) {

		// wrap the angle difference around to be in the range [0, 4*pi]
		while( endRadians - startRadians > 4 * Math.PI )
			endRadians -= 2 * Math.PI;


		// Recurse if angle delta is larger than PI
		if( endRadians - startRadians > Math.PI ) {

			var midRadians = startRadians + (endRadians - startRadians) * 0.5;
			if( forward ) {
				this.arcHelper( path, center, radius, startRadians, midRadians, forward, options );
				this.arcHelper( path, center, radius, midRadians, endRadians, forward, options );
			}
			else {
				this.arcHelper( path, center, radius, midRadians, endRadians, forward, options );
				this.arcHelper( path, center, radius, startRadians, midRadians, forward, options );
			}
		}
		else if( Math.abs( endRadians - startRadians ) > 0.000001 ) {

			var segments = Math.ceil( Math.abs( endRadians - startRadians ) / ( Math.PI / 2.0 ) );
			var angle;
			var angleDelta = ( endRadians - startRadians ) / segments;
			if( forward )
				angle = startRadians;
			else {
				angle = endRadians;
				angleDelta = -angleDelta;
			}

			for( var seg = 0; seg < segments; seg++, angle += angleDelta ) {
				// drawArcSegment
				options.prevPoint = path.getLastSegment().point;
				this.arcSegmentAsCubicBezier( path, center, radius, angle, angle + angleDelta, options );
			}
		}
	},

	arcSegmentAsCubicBezier: function( path, center, radius, startRadians, endRadians, options )
	{
		var r_sin_A, r_cos_A;
		var r_sin_B, r_cos_B;
		var h;

		r_sin_A = radius * Math.sin( startRadians );
		r_cos_A = radius * Math.cos( startRadians );
		r_sin_B = radius * Math.sin( endRadians );
		r_cos_B = radius * Math.cos( endRadians );

		h = 4.0/3.0 * Math.tan( (endRadians - startRadians) / 4 );


		var h1 = new Point( center.x + r_cos_A - h * r_sin_A, center.y + r_sin_A + h * r_cos_A );
		var h2 = new Point( center.x + r_cos_B + h * r_sin_B, center.y + r_sin_B - h * r_cos_B );
		var pt = new Point( center.x + r_cos_B, center.y + r_sin_B );
		// path.cubicCurveTo( h1, h2, pt );

		var h1Pt = new cidocs.HandlePoint( h1, false );
		var h2Pt = new cidocs.HandlePoint( h2, false );
		var ptPt = new cidocs.PathPoint( pt, false );
		this.addExtras( "point", [h1Pt, h2Pt, ptPt] );

		var tempSeg = new cidocs.CubicToSegment( this, [h1Pt, h2Pt, ptPt] );
		options.drawPointText = false;
		this.drawCubicSegment( path, tempSeg, options );
	},

	// coverted from cinder's pathTo
	drawArcToSegment: function( path, segment, options ) {

		if( path.closed || path.isEmpty() ) {
			console.error( "can only arcTo as non-first point" );
			return;
		}

		var epsilon = 1e-8;
		var radius = segment.radius;
		

		// Get current point.
		var p0 = this.getCurrentPoint( path );
		var p1 = new Point( segment.points[0].position );
		var t = new Point( segment.points[1].position );
		
		// Calculate the tangent vectors tangent1 and tangent2.
		var p0t = p0.subtract( t );
		var p1t = p1.subtract( t );
		
		// Calculate tangent distance squares.
		var p0tSquare = p0t.length * p0t.length;
		var p1tSquare = p1t.length * p1t.length;

		// Calculate tan(a/2) where a is the angle between vectors tangent1 and tangent2.
		//
		// Use the following facts:
		//
		//  p0t * p1t  = |p0t| * |p1t| * cos(a) <=> cos(a) =  p0t * p1t  / (|p0t| * |p1t|)
		// |p0t x p1t| = |p0t| * |p1t| * sin(a) <=> sin(a) = |p0t x p1t| / (|p0t| * |p1t|)
		//
		// and
		//
		// tan(a/2) = sin(a) / ( 1 - cos(a) )
		
		var numerator = p0t.y * p1t.x - p1t.y * p0t.x;
		var denominator = Math.sqrt( p0tSquare * p1tSquare ) - ( p0t.x * p1t.x + p0t.y * p1t.y );
		
		// The denominator is zero <=> p0 and p1 are colinear.
		if( Math.abs( denominator ) < epsilon ) {
			// console.log( denominator, epsilon, "LINE" );
			path.lineTo( t );
		}
		else {
			// |b0 - t| = |b3 - t| = radius * tan(a/2).
			var distanceFromT = Math.abs( radius * numerator / denominator );
			
			// b0 = t + |b0 - t| * (p0 - t)/|p0 - t|.
			var b0 = t.add( p0t.normalize().multiply( distanceFromT ) );
			
			// If b0 deviates from p0, add a line to it.
			if( Math.abs(b0.x - p0.x) > epsilon || Math.abs(b0.y - p0.y) > epsilon ) {
				path.lineTo( b0 );
			}
			
			// b3 = t + |b3 - t| * (p1 - t)/|p1 - t|.
			var b3 = t.add( p1t.normalize().multiply( distanceFromT ) );
			
			// The two bezier-control points are located on the tangents at a fraction
			// of the distance[ tangent points <-> tangent intersection ].
			// See "Approxmiation of a Cubic Bezier Curve by Circular Arcs and Vice Versa" by Aleksas Riskus 
			// http://itc.ktu.lt/itc354/Riskus354.pdf
			
			var b0tSquare = (t.x - b0.x) *  (t.x - b0.x) + (t.y - b0.y) *  (t.y - b0.y);
			var radiusSquare = radius * radius;
			var fraction;
			
			// Assume dist = radius = 0 if the radius is very small.
			if( Math.abs( radiusSquare / b0tSquare ) < epsilon )
				fraction = 0.0;
			else
				fraction = ( 4.0 / 3.0 ) / ( 1.0 + Math.sqrt( 1.0 + b0tSquare / radiusSquare ) );
			
			
			var b1 = b0.add( ( t.subtract( b0 ) ).multiply( fraction ) );
			var b2 = b3.add( ( t.subtract( b3 ) ).multiply( fraction ) );
			
			// add the points as path points
			var curPt = new cidocs.PathPoint( this.getCurrentPoint( path ), false );
			var b1Pt = new cidocs.HandlePoint( b1, false );
			var b2Pt = new cidocs.HandlePoint( b2, false );
			var b3Pt = new cidocs.PathPoint( b3, false );
			this.addExtras( "point", [curPt, b1Pt, b2Pt, b3Pt] );
			
			var tempSeg = new cidocs.CubicToSegment( this, [b1Pt, b2Pt, b3Pt] );
			options.drawPointText = false;
			options.prevPoint = curPt.position;
			this.drawCubicSegment( path, tempSeg, options );
		}

		this.drawPointText.call( this, [p1, t] );
	},

	/** SUBDIVIDE */
/*
	void subdivideQuadratic( float distanceToleranceSqr, const vec2 &p1, const vec2 &p2, const vec2 &p3, int level, vector<vec2> *resultPositions, vector<vec2> *resultTangents )
{
	const int recursionLimit = 17;
	const float collinearEpsilon = 0.0000001f;
	
	if( level > recursionLimit ) 
		return;

	vec2 p12 = ( p1 + p2 ) * 0.5f;
	vec2 p23 = ( p2 + p3 ) * 0.5f;
	vec2 p123 = ( p12 + p23 ) * 0.5f;

	float dx = p3.x - p1.x;
	float dy = p3.y - p1.y;
	float d = math<float>::abs(((p2.x - p3.x) * dy - (p2.y - p3.y) * dx));

	if( d > collinearEpsilon ) { 
		if( d * d <= distanceToleranceSqr * (dx*dx + dy*dy) ) {
			resultPositions->emplace_back( p123 );
			if( resultTangents )
				resultTangents->emplace_back( p3 - p1 );
			return;
		}
	}
	else { // Collinear case
		float da = dx * dx + dy * dy;
		if( da == 0 ) {
			d = distance2( p1, p2 );
		}
		else {
			d = ((p2.x - p1.x)*dx + (p2.y - p1.y)*dy) / da;
			if( d > 0 && d < 1 ) {
				// Simple collinear case, 1---2---3 - We can leave just two endpoints
				return;
			}
			
			if(d <= 0)
				d = distance2( p2, p1 );
			else if(d >= 1)
				d = distance2( p2, p3 );
			else
				d = distance2( p2, vec2( p1.x + d * dx, p1.y + d * dy ) );
		}
		if( d < distanceToleranceSqr ) {
			resultPositions->emplace_back( p2 );
			if( resultTangents )
				resultTangents->emplace_back( p3 - p1 );
			return;
		}
	}

	// Continue subdivision
	subdivideQuadratic( distanceToleranceSqr, p1, p12, p123, level + 1, resultPositions, resultTangents );
	subdivideQuadratic( distanceToleranceSqr, p123, p23, p3, level + 1, resultPositions, resultTangents );
}*/

	// This technique is due to Maxim Shemanarev but removes his tangent error estimates
	subdivideCubic: function( distanceToleranceSqr, p1, p2, p3, p4, level, resultPositions, resultTangents ) {
		var recursionLimit = 17;
		var collinearEpsilon = 0.0000001;
		
		if( level > recursionLimit ) 
			return;
		
		// Calculate all the mid-points of the line segments
		//----------------------

		var p12 = ( p1.add( p2 ) ).multiply( 0.5 );
		var p23 = ( p2.add( p3 ) ).multiply( 0.5 );
		var p34 = ( p3.add( p4 ) ).multiply( 0.5 );
		var p123 = ( p12.add( p23 ) ).multiply( 0.5 );
		var p234 = ( p23.add( p34 ) ).multiply( 0.5 );
		var p1234 = ( p123.add( p234 ) ).multiply( 0.5 );


		// Try to approximate the full cubic curve by a single straight line
		//------------------
		var dx = p4.x - p1.x;
		var dy = p4.y - p1.y;

		var d2 = Math.abs(((p2.x - p4.x) * dy - (p2.y - p4.y) * dx));
		var d3 = Math.abs(((p3.x - p4.x) * dy - (p3.y - p4.y) * dx));
		var k, da1, da2;

		var collinear = ( (+(d2 > collinearEpsilon)) << 1) + (+(d3 > collinearEpsilon));
		switch( collinear ) {

			case 0:
				// All collinear OR p1==p4
				k = dx*dx + dy*dy;
				if( k == 0 ) {
					d2 = distance2( p1, p2 );
					d3 = distance2( p4, p3 );
				}
				else {
					k   = 1 / k;
					da1 = p2.x - p1.x;
					da2 = p2.y - p1.y;
					d2  = k * ( da1 * dx + da2 * dy );
					da1 = p3.x - p1.x;
					da2 = p3.y - p1.y;
					d3  = k * ( da1 * dx + da2 * dy );
					if( d2 > 0 && d2 < 1 && d3 > 0 && d3 < 1 ) {
						// Simple collinear case, 1---2---3---4
						// We can leave just two endpoints
						return;
					}
						 if(d2 <= 0) d2 = distance2( p2, p1 );
					else if(d2 >= 1) d2 = distance2( p2, p4 );
					else             d2 = distance2( p2, new Point( p1.x + d2*dx, p1.y + d2*dy ) );

						 if(d3 <= 0) d3 = distance2( p3, p1 );
					else if(d3 >= 1) d3 = distance2( p3, p4 );
					else             d3 = distance2( p3, new Point( p1.x + d3*dx, p1.y + d3*dy ) );
				}
				if(d2 > d3) {
					if( d2 < distanceToleranceSqr ) {
						resultPositions.push( p2 );
						if( resultTangents )
							resultTangents.push( p3.subtract( p1 ) );
						return;
					}
				}
				else {
					if( d3 < distanceToleranceSqr ) {
						resultPositions.push( p3 );
						if( resultTangents )
							resultTangents.push( p4.subtract( p2 ) );
						return;
					}
				}
			break;
			case 1:
				// p1,p2,p4 are collinear, p3 is significant
				if( d3 * d3 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
					resultPositions.push( p23 );
					if( resultTangents )
						resultTangents.push( p3.subtract( p2 ) );
					return;
				}
			break;
			case 2:
				// p1,p3,p4 are collinear, p2 is significant
				if( d2 * d2 <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
					resultPositions.push( p23 );
					if( resultTangents )
						resultTangents.push( p3.subtract( p2 ) );
					return;
				}
			break;
			case 3: 
				// Regular case
				if( (d2 + d3)*(d2 + d3) <= distanceToleranceSqr * ( dx*dx + dy*dy ) ) {
					resultPositions.push( p23 );
					if( resultTangents )
						resultTangents.push( p3.subtract( p2 ) );
					return;
				}
			break;
		}

		// Continue subdivision
		this.subdivideCubic( distanceToleranceSqr, p1, p12, p123, p1234, level + 1, resultPositions, resultTangents );
		this.subdivideCubic( distanceToleranceSqr, p1234, p234, p34, p4, level + 1, resultPositions, resultTangents );
	},

	
	subdivide: function( approximationScale ) {
		if( ! this.segs.length )
			return;

		var resultPositions = [];
		var resultTangents = [];
		var distanceToleranceSqr = 0.5 / approximationScale;
		distanceToleranceSqr *= distanceToleranceSqr;
		
		var firstPoint = 0;
		resultPositions.push( this.points[0].position );
		if( resultTangents )
			resultTangents.push( this.points[1].position.subtract( this.points[0].position ) );

		for( var s = 1; s < this.segs.length; ++s ) {
			switch( this.segs[s].type ) {
				case SEGMENT_TYPES.CUBICTO:
					resultPositions.push( this.points[firstPoint].position );
					if( resultTangents )
						resultTangents.push( this.points[firstPoint+1].position - this.points[firstPoint].position );
					this.subdivideCubic( distanceToleranceSqr, this.points[firstPoint].position, this.points[firstPoint+1].position, this.points[firstPoint+2].position, this.points[firstPoint+3].position, 0, resultPositions, resultTangents );
					resultPositions.push( this.points[firstPoint+3].position );
					if( resultTangents )
					 	resultTangents.push( this.points[firstPoint+3].position - this.points[firstPoint+2].position );
				break;
				case SEGMENT_TYPES.QUADTO:
					// resultPositions->emplace_back( mPoints[firstPoint] );
					// if( resultTangents )
					// 	resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
					// subdivideQuadratic( distanceToleranceSqr, mPoints[firstPoint], mPoints[firstPoint+1], mPoints[firstPoint+2], 0, resultPositions, resultTangents );
					// resultPositions->emplace_back( mPoints[firstPoint+2] );
					// if( resultTangents )
					// 	resultTangents->emplace_back( mPoints[firstPoint+2] - mPoints[firstPoint+1] );
				break;
				case SEGMENT_TYPES.LINETO:
					// resultPositions->emplace_back( mPoints[firstPoint] );
					// if( resultTangents )
					// 	resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
					// resultPositions->emplace_back( mPoints[firstPoint+1] );
					// if( resultTangents )
					// 	resultTangents->emplace_back( mPoints[firstPoint+1] - mPoints[firstPoint] );
				break;
				case SEGMENT_TYPES.CLOSE:
					// resultPositions->emplace_back( mPoints[firstPoint] );
					// if( resultTangents )
					// 	resultTangents->emplace_back( mPoints[0] - mPoints[firstPoint] );
					// resultPositions->emplace_back( mPoints[0] );
					// if( resultTangents )
					// 	resultTangents->emplace_back( mPoints[0] - mPoints[firstPoint] );
				break;
				default:
					throw "Unexpected segment type: " + this.segs[s].type;
			}
			
			firstPoint += SEGMENT_POINT_COUNTS[this.segs[s].type];
		}


		return resultPositions;
	},


	calcBoundingBox: function()
	{
		return this.path.handleBounds;
	},

	calcPreciseBoundingBox: function()
	{
		return this.path.bounds;
	},

	contains: function( pt )
	{
		return this.path.contains( pt );
	},

	/** Returns a copy of the path2d's path, not a path2d copy */
	transformCopyPath: function( mtrx ){
		var pathCopy = this.path.clone();
		pathCopy.transform( mtrx );
		return pathCopy;
	},

	drawLineExtra: function( pt1, pt2 )
	{
		var l1 = new Path.Line( pt1, pt2 );
		l1.strokeColor = 'cyan';
		l1.sendToBack();
		this.addExtras( "line-extra", [l1] );
	},

	addExtras: function( type, items ) {

		var self = this;

		_.each( items, function( item ){
			item._extraType = type;
			self.extras.push( item );
		} );

		// bring certain points to the front
		_.each( this.extras, function( extra ){
			if( extra._extraType === "point" || extra._extraType === "handle-line" ){
				extra.bringToFront();
			}
		});

		return this.extras;
	},

	close: function() {


		var segment = new cidocs.CloseSegment( this );
		this.segs.push( segment );

		this.drawPath();
	},

	setPosition: function( pos ) {

		var tlPos = this.path.bounds.topLeft;
		_.each( this.points, function( pt ) {
			var relPos = (new Point(pt.position)).subtract( new Point( tlPos ) );
			pt.position.x = Math.round( relPos.x + pos.x );
			pt.position.y = Math.round( relPos.y + pos.y );
		} );


		this.drawPath();
	},

	centerInCanvas: function( canvas ) {

		var posX = Math.round( (canvas[0].clientWidth - this.path.bounds.width) / 2 );
		var posY = Math.round( (canvas[0].clientHeight - this.path.bounds.height) / 2 );
		this.setPosition( new Point( posX, posY ) );
	},

	movePoint: function( selectedPoint, pos ) {

		var ptIndex = 0;
		var points = this.points;
		var self = this;
		var mainPt = null;
		var newPos = pos.round();

		_.each( this.segs, function( segment, index, segments ) {

			if( mainPt )
				return;

			var ptsToMove = [];
			var nextSegment = (segments[index+1]) ? segments[index+1].type : null;

			switch( segment.type ) {
				
				case SEGMENT_TYPES.MOVETO:
					mainPt = ptIndex;
					ptsToMove = [points[mainPt]];
					if( nextSegment === SEGMENT_TYPES.CUBICTO ) {
						ptsToMove.push( points[ptIndex + 1] );
					}
					ptIndex++;
					break;

				case SEGMENT_TYPES.LINETO:
					mainPt = ptIndex;
					ptsToMove = [points[mainPt]];
					if( nextSegment === SEGMENT_TYPES.CUBICTO ) {
						ptsToMove.push( points[ptIndex + 1] );
					}
					ptIndex++;
					break;

				case SEGMENT_TYPES.QUADTO:
					mainPt = ptIndex + 1;
					ptsToMove = [points[mainPt]];
					if( nextSegment === SEGMENT_TYPES.CUBICTO ) {
						ptsToMove.push( points[ptIndex + 2] );
					}
					ptIndex+=2;
					break;

				case SEGMENT_TYPES.CUBICTO:
					mainPt = ptIndex + 2;
					ptsToMove = [points[mainPt], points[ptIndex + 1]];
					if( nextSegment === SEGMENT_TYPES.CUBICTO ) {
						ptsToMove.push( points[ptIndex + 3] );
					}
					ptIndex+=3;
					break;

				case SEGMENT_TYPES.ARC:
					
					mainPt = ptIndex;
					ptsToMove = [points[mainPt]];

					var center = points[ptIndex];
					var pt1 = points[ptIndex + 1];
					var pt2 = points[ptIndex + 2];

					var rad, angle1, angle2, startPt, endPt;
					if( selectedPoint == pt1 || selectedPoint == pt2 ) {
						if( selectedPoint == pt1 ) {
							rad = pt1.position.getDistance( center.position );
						}else if (selectedPoint == pt2 ) {
							rad = pt2.position.getDistance( center.position );
						}

						angle1 = pt1.position.subtract( center.position ).angleInRadians;
						angle2 = pt2.position.subtract( center.position ).angleInRadians;

						startPt = new Point( Math.round( Math.cos( angle1 ) * rad ),
							Math.round( Math.sin( angle1 ) * rad ) ).add( center.position );
						endPt = new Point( Math.round( Math.cos( angle2 ) * rad ),
							Math.round( Math.sin( angle2 ) * rad ) ).add( center.position );

						pt1.position = startPt;
						pt2.position = endPt;

						segment.radius = rad;
						segment.startRadians = angle1;
						segment.endRadians = angle2;
					}
					else{

						ptsToMove = [center, pt1, pt2];
					}

					if( nextSegment === SEGMENT_TYPES.CUBICTO ) {
						ptsToMove.push( points[ptIndex + 3] );
					}
					ptIndex+=3;
					break;
			}

			// if we've found the point to move, move it aling with companion points
			if( selectedPoint === self.points[mainPt] ) {
				var mainPt = new Point( points[mainPt].position );
				var diff = newPos.subtract( mainPt );

				_.each( ptsToMove, function( pt ) {
					pt.position = new Point( pt.position ).add( diff );
				}, this );
			}
		} );

		if( ! mainPt ) {
			selectedPoint.position = newPos;
		}
	},

	convertPoints: function( points ){

		var paperPoints = [];
		var self = this;

		_.each( points, function( pt ){
			paperPoints.push( new self.ps.Point( pt.x, pt.y ) );
		});

		return paperPoints;
	},

	getCinderCode: function() {

		var code = "";

		_.each( this.segs, function( segment ){
			code += segment.getCode();
		});
		
		return code;
	},

	handlesOn: function() {
		_.each( this.points, function( point ) {
			point.visible = true;
		} );

		_.each( this.extras, function( extra ) {
			extra.visible = true;
		} );
	},

	handlesOff: function() {
		_.each( this.points, function( point ) {
			point.visible = false;
		} );
		_.each( this.extras, function( extra ) {
			extra.visible = false;
		} );

	},

	drawPath: function() {

		this.ps.activate();

		// redraw the path using the original directions
		var ptIndex = 0;
		var points = this.points;
		var self = this;

		// reset the path and send to below the points
		this.path.remove();
		this.path = new Path();
		self.path.strokeColor = COLOR_PATH;
		this.path.sendToBack();

		// lines between handles and points
		_.each( this.extras, function( extra ) {
			extra.remove();
		});
		this.extras = [];

		_.each( this.segs, function( segment, index ) {

			var segmentPoints = segment.points;
			var prevPoint = ( ptIndex === 0 ) ? null : points[ptIndex-1].position;
			var pt = null;
			// console.log( segment.type );
			// segment.draw();

			switch( segment.type ) {
				
				case SEGMENT_TYPES.MOVETO:
					
					var pt = segmentPoints[0].position;
					self.path.moveTo( new Point( pt ) );
					self.drawPointText.call( self, [pt] );
					ptIndex++;
					break;

				case SEGMENT_TYPES.LINETO:
					
					var pt = segmentPoints[0].position;

					// path line
					self.path.lineTo( new Point( pt ) );

					// segment line
					var seg = new Path();
					seg.moveTo( prevPoint );
					seg.strokeColor = COLOR_LINE_TO;
					seg.strokeWidth = 3.0;
					seg.lineTo( new Point( pt ) );
					self.addExtras( "segment", [seg] );

					self.drawPointText.call( self, [pt] );
					ptIndex++;
					break;

				case SEGMENT_TYPES.QUADTO:

					var h1 = segmentPoints[0].position;
					var pt1 = segmentPoints[1].position;

					var l1 = new Path.Line( new Point( h1 ), new Point( pt1 ) );
					var l2 = new Path.Line( new Point( h1 ), new Point( prevPoint ) );
					l1.strokeColor = 'cyan';
					l2.strokeColor = 'cyan';
					l1.sendToBack();
					l2.sendToBack();
					self.addExtras( "handle-line", [l1, l2] );

					self.path.quadraticCurveTo(
						new Point( h1 ),
						new Point( pt1 )
					);

					
					// segment line
					var seg = new Path();
					seg.moveTo( prevPoint );
					var strokeColor = segmentPoints[0].active ? COLOR_QUAD_TO : COLOR_INACTIVE;
					seg.strokeColor = strokeColor;
					seg.strokeWidth = 3.0;
					seg.quadraticCurveTo(
						new Point( h1 ),
						new Point( pt1 )
					);
					self.addExtras( "segment", [seg] );

					self.drawPointText.call( self, [h1, pt1] );
					ptIndex+=2;
					break;

				case SEGMENT_TYPES.CUBICTO:

					var seg = new Path();
					seg.moveTo( prevPoint );
					seg.strokeColor = COLOR_CUBIC_TO;
					seg.strokeWidth = 3.0;
					self.addExtras( "segment", [seg] );

					self.drawCubicSegment( self.path, segment, { prevPoint: prevPoint, extras:true } );
					self.drawCubicSegment( seg, segment, prevPoint, { prevPoint: prevPoint, extras:false } );

					ptIndex+=3;
					break;

				case SEGMENT_TYPES.ARC:

					// main path segment
					self.drawArcSegment( self.path, segment, { prevPoint: prevPoint, extras:true } );

					// special segment
					var seg = new Path();
					seg.strokeColor = COLOR_CUBIC_TO;
					seg.strokeWidth = 3.0;
					self.drawArcSegment( seg, segment, { prevPoint: prevPoint, extras:false } );
					self.addExtras( "segment", [seg] );

					ptIndex += 3;

					break;

				case SEGMENT_TYPES.ARCTO:

					// main path segment
					self.drawArcToSegment( self.path, segment, { prevPoint: prevPoint, extras:true } );

					// overlay segment
					var seg = new Path();
					seg.moveTo( prevPoint );
					seg.strokeColor = COLOR_CUBIC_TO;
					seg.strokeWidth = 3.0;
					self.drawArcToSegment( seg, segment, { prevPoint: prevPoint, extras:false } );
					self.addExtras( "segment", [seg] );

					ptIndex += segment.points.length;

					break;

				case SEGMENT_TYPES.CLOSE:

					self.path.closed = true;

					// segment line
					var seg = new Path();
					seg.strokeColor = COLOR_CLOSE;
					seg.strokeWidth = 3.0;
					seg.moveTo( prevPoint );
					seg.lineTo(
						points[0].position
					);
					self.addExtras( "segment", [seg] );
					break;
			}
		} );
		

		_.each( this.points, function( point ) {
			point.bringToFront();
		});

		// console.log( "PATH", this.path );
	},

	drawPointText: function( pts ) {

		var self = this;
		_.each( pts, function( pt ){
			var text = new PointText({
				point: [pt.x + 5, pt.y],
				content: '[' + pt.x + ', ' + pt.y + ']',
				fillColor: 'black',
				fontFamily: 'Courier New',
				fontSize: 8
			});
			self.addExtras( "text", [text] );
		});
	},

	drawTextAtPoint: function( pt, text ) {

		var self = this;
		var ptText = new PointText( {
			point: [pt.x + 5, pt.y],
			content: text,
			fillColor: 'black',
			fontFamily: 'Courier New',
			fontSize: 8
		});
		self.addExtras( "text", [ptText] );
	},

	getCurrentPoint: function( path ) {

		return path.getLastSegment().point;
	},

	reset: function() {

		_.each( this.points, function( point ) {
			point.remove();
		});

		_.each( this.extras, function( extra ) {
			extra.remove();
		});

		this.path.remove();
		this.points		= [];
		this.segs		= [];
		this.extras		= [];

		
	}
};

// +———————————————————————————————————————+
//	Path2dSketch     
//	all sketches should extend this
// +———————————————————————————————————————+

cidocs.Path2dSketch = function( options ) {

	this.sketch = null;
	this.canvas = null;
	this.output = null;
	this.paths = [];
	this.extras = [];
	this.curPaper = null;
	this.tool = null;
	this.selectedSegment = null;
	this.selectedPath = null;
	this.selectedPoint = null;
	this.hitOptions = {
		segments: true,
		stroke: true,
		fill: true,
		tolerance: 5
	};
	this.movePath = false;
	this.handlesOn = true;
	this.buttons = [];
	this.gui = new dat.GUI( { autoPlace: false } );

	this.initialize( options );
};

cidocs.Path2dSketch.prototype = {

	initialize: function( opts ) {

		var options = opts || {};

		// use defined options
		this.canvas = $( options.canvas );
		this.output = options.output;
		this.name 	= options.name;

		var paperScope = new paper.PaperScope();
		paperScope.setup( this.canvas[0] );
		this.curPaper = paperScope;

		this.tool = new paperScope.Tool();
		this.tool.onMouseMove = _.bind( this.onToolMouseMove, this );
		this.tool.onMouseDown = _.bind( this.onToolMouseDown, this );
		this.tool.onMouseDrag = _.bind( this.onToolMouseDrag, this );

		// wrap canvas in div
		this.gui.name = 'gui_' + this.name;
		// $(this.gui.domElement);//.addClass( this.name );
		$(this.gui.domElement).find('.close-button').remove();
		var canvasWrapper = $("<div class='sketch'></div>");
		canvasWrapper.addClass( options.name );
		canvasWrapper.append( this.canvas );
		canvasWrapper.append( this.gui.domElement );
		$("#tutorial .canvases").append( canvasWrapper );
		this.sketch = canvasWrapper;
	},

	onToolMouseMove: function( event ) {

		this.curPaper.project.activeLayer.selected = false;
		/*console.log( event.item );
		if( event.item ) {
			event.item.selected = true;
		}*/

		/*var result = this.curPaper.project.hitTest( event.point, this.hitOptions );
		console.log( result );
		if( result ) {
			// if( result == this)
			// result.item._parent.selected = true;
			result.item._parent.fullySelected = true;
		}*/

		var hitResult = this.curPaper.project.hitTest( event.point, this.hitOptions );
		var active = ( hitResult && _.isBoolean( hitResult.item.active ) ) ? hitResult.item.active : undefined;
		
		// if( hitResult && active !== false && ( hitResult.item.type === 'rectangle' || hitResult.type === "segment" ) ){
		if( hitResult && active  ){
			hitResult.item.fullySelected = true;
		}

	},

	onToolMouseDown: function( event ) {

		this.selectedSegment = this.selectedPath = this.selectedPoint = null;
		var hitResult = this.curPaper.project.hitTest( event.point, this.hitOptions );
		if( !hitResult )
			return;

		if( hitResult ) {
			// this.selectedPath = hitResult.item;
			// this.selectedPath.fullySelected = true;

			// console.log( "MOUSE DOWN", hitResult.item.type );
			/*if (hitResult.type == 'segment') {
				this.selectedSegment = hitResult.segment;
			}*/

			// if (hitResult.item.type === 'rectangle' || hitResult.type === "segment") {
			// if( hitResult.item.active !== false && (hitResult.item.type === 'rectangle' || hitResult.type === "segment") ) {
			if( hitResult.item.active ) {
				// console.log( "SELECT ME" )
				this.selectedPoint = hitResult.item;
			}
		}
	},

	onToolMouseDrag: function( event ) {
		
		if( this.selectedPoint ) {

			this.paths[0].movePoint( this.selectedPoint, event.point );
			this.drawPath();
			this.updateSketch();
		}
	},

	drawInitialPath: function() {
		// overwrite
	},

	drawBoundingBox: function() {
		var boundingBox = new cidocs.BoundingBox( this.paths[0] );
		this.extras.push( boundingBox );
	},

	drawPreciseBoundingBox: function() {
		var boundingBox = new cidocs.PreciseBoundingBox( this.paths[0] );
		this.extras.push( boundingBox );
	},

	contains: function( pt ) {
		return this.paths[0].contains( pt );
	},

	reset: function(){

		this.paths[0].reset();
		_.each( this.extras, function( extra ) {
			extra.reset();
		} );
		this.paths = [];
		this.extras = [];
		this.drawInitialPath();

		if( ! this.handlesOn ){
			_.each( this.paths, function( path ) {
				path.handlesOff();
			} );
		}

		this.drawPath();
		this.updateSketch();
	},

	updateSketch: function() {

		this.curPaper.view.draw();
		this.output.update( this.paths[0], this.extras );
	},

	toggleHandles: function() {

		this.handlesOn = !this.handlesOn;

		if( this.handlesOn ){
			_.each( this.paths, function( path ) {
				path.handlesOn();
			} );
		} else {
			_.each( this.paths, function( path ) {
				path.handlesOff();
			} );
		}

		this.updateSketch();
	},

	show: function() {

		this.sketch.removeClass( "inactive" );
		this.sketch.addClass( "active" );
		
		this.drawPath();

		if( ! this.handlesOn ){
			_.each( this.paths, function( path ) {
				path.handlesOff();
			} );
		}
		
		this.updateSketch();

		_.bindAll( this, 'toggleHandles', 'reset' );
		$("#handle-toggle").click( $.proxy( this.toggleHandles, this) );
		$("#reset").click( $.proxy( this.reset, this) );

		// show buttons
		_.each( this.buttons, function( button ) {
			button.removeClass( 'invisible' );
		} );
	},

	hide: function() {

		this.sketch.removeClass( "active" );
		this.sketch.addClass( "inactive" );
		$( "#handle-toggle" ).unbind( 'click' );
		$( "#reset" ).unbind( 'click' );

		// hide buttons
		// show buttons
		_.each( this.buttons, function( button ) {
			button.addClass( 'invisible' );
		} );
	},

	addButton: function( id, name, options ) {

		var exists = _.find( $('.canvasButtons .right button'), function( button ) { return button.id === id;  } );

		// add to dat-gui
		// var rangeMin = ( options && options.min ) options || null;
		// var rangeMax = options.max || null;
		// var step = options.step || 0.05;

		var rangeMin 	= ( options && options.min ) ? options.min : null;
		var rangeMax 	= ( options && options.max ) ? options.max : null;
		var step 		= ( options && options.step ) ? options.step : null;
		var btn 		= this.gui.add( this, id, rangeMin, rangeMax );
		if( step ) btn.step(step);

		btn.name( name );
		btn.updateDisplay();
	},

	activateButton: function( buttonId, func ) {

		var btn = _.find( this.buttons, function( btn ){ return btn[0].id === buttonId; } );
		if( btn ) {
			btn.click( $.proxy( func, this) );
		}
	},

	deactivateButton: function( buttonId ) {

		var btn = _.find( this.buttons, function( btn ){ return btn[0].id === buttonId; } );
		if( btn ) {
			btn.unbind('click');
		}
	},

	drawPath: function() {

		this.paths[0].drawPath();

		_.every( this.extras, function( extra ){
			extra.draw();
		} );
	}
};


// +———————————————————————————————————————+
//	Extra drawing commands     
//	Additional commands for the sketch
//	such as drawBoundbox calls
// +———————————————————————————————————————+

cidocs.BoundingBox = function( path2d ) {
	this.path2d = path2d;
	this.box = null;
	this.template = _.template( "gl::drawStrokedRect( mPath.calcBoundingBox() );" );
};

cidocs.BoundingBox.prototype = {

	draw: function() {
		if( this.box ) {
			this.box.remove();
		}
		var bounds = this.path2d.calcBoundingBox();
		var box = new Path.Rectangle( bounds );
		box.strokeColor = COLOR_INACTIVE;
		this.box = box;
	},

	getCinderCode: function() {
		return this.template();
	},

	reset: function() {
		this.box.remove();
		this.box = null;
	}
};

cidocs.PreciseBoundingBox = function( path2d ) {
	this.path2d = path2d;
	this.template = "";
	this.box = null;
	this.template = _.template( "gl::drawStrokedRect( mPath.calcPreciseBoundingBox() );" );
};

cidocs.PreciseBoundingBox.prototype = {

	draw: function() {
		if( this.box ) {
			this.box.remove();
		}
		var bounds = this.path2d.calcPreciseBoundingBox();
		var box = new Path.Rectangle( bounds );
		box.strokeColor = COLOR_INACTIVE;
		this.box = box;
	},

	getCinderCode: function() {
	return this.template();
	},

	reset: function() {
		this.box.remove();
		this.box = null;
	}
};

// +———————————————————————————————————————+
//	Path2dCode     
//	Code area that displays code for a path
// +———————————————————————————————————————+

cidocs.CodeModule = function() {

	this.div = null;
	this.code = "";

	this.init = function() {
		this.div = $( '#output' );
	};

	this.update = function( path, extras ) {

		var code = "";

		if( path ) {
			var segments = path.segments;
			var p = "mPath";
			code = "Path2d mPath;\n";
			code += path.getCinderCode();
			code += "gl::draw( mPath );\n";
		}

		if( extras.length ) {
			_.each( extras, function( extra ) {
				code += extra.getCinderCode();
			} );
		}

		this.code = code;
		this.div.html( Prism.highlight( this.code, Prism.languages.cpp ) );
	};

	this.inject = function( str ) {

		this.code += str;
		this.div.html( Prism.highlight( this.code, Prism.languages.cpp ) );
	};

	this.prepend = function( str ) {

		this.code = str + this.code;
		this.div.html( Prism.highlight( this.code, Prism.languages.cpp ) );
	}

	this.init();
};


// +———————————————————————————————————————+
//	Main App
// +———————————————————————————————————————+

cidocs.app = function() {

	this.sketches = [];

	this.codeModule = new cidocs.CodeModule();
};

cidocs.app.prototype = {

	init: function(){
		
		// extended class will define sketches
		var self = this;
		var linksDiv = $( '#sketchLinks' );
		_.each( this.sketches, function( sketch ){
			// create a link

			var li = $('<li>').appendTo( linksDiv );
			var link = $('<a>', {
				text: sketch.canvas.data('name'),
				title: 'Blah',
				href: '#',
				'data-sketch': sketch.canvas[0].id,
				click: function(){ self.show( sketch.canvas[0].id );return false;}
			});
			link.appendTo( li );
		});
	},

	show: function( moduleName ){

		// remove active from any active canvases
		var elements = document.querySelectorAll( "canvas" );

		_.each( this.sketches, function( sketch, i ){
			sketch.hide();
		});


		// make the specified canvas active
		_.findWhere( this.sketches, {name:moduleName}).show();
		_.each( $( '#sketchLinks a' ), function( el ){
			var $el = $( el );
			if( $el.data('sketch') === moduleName )
				$el.addClass( 'active' );
			else
				$el.removeClass( 'active' );
		} );
	}
};



$(document).ready(function(){

    
    $('a#copy-button').zclip( {
        path: 'ZeroClipboard.swf',
        copy: function(){ 
            console.log( "COPY", $( 'code#output' ).text() );
            var output = $( 'code#output' ).text();
            return output;
        },
        beforeCopy: function(){
            console.log( "BEFORE COPY" );
            return "";
        },
        afterCopy: function(){
           console.log( "COPIED", $( 'code#output' ).text() );
           return "";
        }                           
    });

});
                
/*! iFrame Resizer (iframeSizer.contentWindow.min.js) - v3.1.0 - 2015-08-21
 *  Desc: Include this file in any page being loaded into an iframe
 *        to force the iframe to resize to the content size.
 *  Requires: iframeResizer.min.js on host page.
 *  Copyright: (c) 2015 David J. Bradshaw - dave@bradshaw.net
 *  License: MIT
 */

!function(a){"use strict";function b(b,c,d){"addEventListener"in a?b.addEventListener(c,d,!1):"attachEvent"in a&&b.attachEvent("on"+c,d)}function c(a){var b,c,d,e=null,f=0,g=function(){f=rb(),e=null,d=a.apply(b,c),e||(b=c=null)};return function(){var h=rb();f||(f=h);var i=mb-(h-f);return b=this,c=arguments,0>=i||i>mb?(e&&(clearTimeout(e),e=null),f=h,d=a.apply(b,c),e||(b=c=null)):e||(e=setTimeout(g,i)),d}}function d(a){return cb+"["+eb+"] "+a}function e(b){bb&&"object"==typeof a.console&&console.log(d(b))}function f(b){"object"==typeof a.console&&console.warn(d(b))}function g(){h(),e("Initialising iFrame ("+location.href+")"),i(),l(),k("background",O),k("padding",R),t(),q(),r(),m(),v(),s(),_=u(),F("init","Init message from host page")}function h(){function a(a){return"true"===a?!0:!1}var b=$.substr(db).split(":");eb=b[0],P=void 0!==b[1]?Number(b[1]):P,S=void 0!==b[2]?a(b[2]):S,bb=void 0!==b[3]?a(b[3]):bb,ab=void 0!==b[4]?Number(b[4]):ab,M=void 0!==b[6]?a(b[6]):M,Q=b[7],Y=void 0!==b[8]?b[8]:Y,O=b[9],R=b[10],jb=void 0!==b[11]?Number(b[11]):jb,_.enable=void 0!==b[12]?a(b[12]):!1,gb=void 0!==b[13]?b[13]:gb,pb=void 0!==b[14]?b[14]:pb}function i(){function b(){var b=a.iFrameResizer;e("Reading data from page: "+JSON.stringify(b)),qb=void 0!==b.messageCallback?b.messageCallback:qb,hb=void 0!==b.targetOrigin?b.targetOrigin:hb,Y=void 0!==b.heightCalculationMethod?b.heightCalculationMethod:Y,pb=void 0!==b.widthCalculationMethod?b.widthCalculationMethod:pb}"iFrameResizer"in a&&Object===a.iFrameResizer.constructor&&b()}function j(a,b){return-1!==b.indexOf("-")&&(f("Negative CSS value ignored for "+a),b=""),b}function k(a,b){void 0!==b&&""!==b&&"null"!==b&&(document.body.style[a]=b,e("Body "+a+' set to "'+b+'"'))}function l(){void 0===Q&&(Q=P+"px"),j("margin",Q),k("margin",Q)}function m(){document.documentElement.style.height="",document.body.style.height="",e('HTML & body height set to "auto"')}function n(c){function d(d){function e(){F(c.eventName,c.eventType)}b(a,d,e)}c.eventNames&&Array.prototype.map?(c.eventName=c.eventNames[0],c.eventNames.map(d)):d(c.eventName),e("Added event listener: "+c.eventType)}function o(){n({eventType:"Animation Start",eventNames:["animationstart","webkitAnimationStart"]}),n({eventType:"Animation Iteration",eventNames:["animationiteration","webkitAnimationIteration"]}),n({eventType:"Animation End",eventNames:["animationend","webkitAnimationEnd"]}),n({eventType:"Orientation Change",eventName:"orientationchange"}),n({eventType:"Input",eventName:"input"}),n({eventType:"Print",eventName:["afterprint","beforeprint"]}),n({eventType:"Transition End",eventNames:["transitionend","webkitTransitionEnd","MSTransitionEnd","oTransitionEnd","otransitionend"]}),n({eventType:"Mouse Up",eventName:"mouseup"}),n({eventType:"Mouse Down",eventName:"mousedown"}),"child"===gb&&n({eventType:"IFrame Resized",eventName:"resize"})}function p(a,b,c,d){b!==a&&(a in c||(f(a+" is not a valid option for "+d+"CalculationMethod."),a=b),e(d+' calculation method set to "'+a+'"'))}function q(){p(Y,X,sb,"height")}function r(){p(pb,ob,tb,"width")}function s(){!0===M?(o(),y()):e("Auto Resize disabled")}function t(){var a=document.createElement("div");a.style.clear="both",a.style.display="block",document.body.appendChild(a)}function u(){function c(){return{x:void 0!==a.pageXOffset?a.pageXOffset:document.documentElement.scrollLeft,y:void 0!==a.pageYOffset?a.pageYOffset:document.documentElement.scrollTop}}function d(a){var b=a.getBoundingClientRect(),d=c();return{x:parseInt(b.left,10)+parseInt(d.x,10),y:parseInt(b.top,10)+parseInt(d.y,10)}}function g(a){function b(a){var b=d(a);e("Moving to in page link (#"+c+") at x: "+b.x+" y: "+b.y),J(b.y,b.x,"scrollToOffset")}var c=a.split("#")[1]||a,f=decodeURIComponent(c),g=document.getElementById(f)||document.getElementsByName(f)[0];g?b(g):(e("In page link (#"+c+") not found in iFrame, so sending to parent"),J(0,0,"inPageLink","#"+c))}function h(){""!==location.hash&&"#"!==location.hash&&g(location.href)}function i(){function a(a){function c(a){a.preventDefault(),g(this.getAttribute("href"))}"#"!==a.getAttribute("href")&&b(a,"click",c)}Array.prototype.forEach.call(document.querySelectorAll('a[href^="#"]'),a)}function j(){b(a,"hashchange",h)}function k(){setTimeout(h,U)}function l(){Array.prototype.forEach&&document.querySelectorAll?(e("Setting up location.hash handlers"),i(),j(),k()):f("In page linking not fully supported in this browser! (See README.md for IE8 workaround)")}return _.enable?l():e("In page linking not enabled"),{findTarget:g}}function v(){e("Enable public methods"),a.parentIFrame={close:function(){J(0,0,"close")},getId:function(){return eb},moveToAnchor:function(a){_.findTarget(a)},reset:function(){I("parentIFrame.reset")},scrollTo:function(a,b){J(b,a,"scrollTo")},scrollToOffset:function(a,b){J(b,a,"scrollToOffset")},sendMessage:function(a,b){J(0,0,"message",JSON.stringify(a),b)},setHeightCalculationMethod:function(a){Y=a,q()},setWidthCalculationMethod:function(a){pb=a,r()},setTargetOrigin:function(a){e("Set targetOrigin: "+a),hb=a},size:function(a,b){var c=""+(a?a:"")+(b?","+b:"");G(),F("size","parentIFrame.size("+c+")",a,b)}}}function w(){0!==ab&&(e("setInterval: "+ab+"ms"),setInterval(function(){F("interval","setInterval: "+ab)},Math.abs(ab)))}function x(a){return void 0===a||0===a}function y(){function b(a){function b(a){var b=F.bind(null,"imageLoad","Image loaded",void 0,void 0);(x(a.height)||x(a.width))&&(e("Attach listerner to "+a.src),a.addEventListener("load",b,!1))}"attributes"===a.type&&"src"===a.attributeName?b(a.target):"childList"===a.type&&Array.prototype.forEach.call(a.target.querySelectorAll("img"),b)}function c(a){F("mutationObserver","mutationObserver: "+a[0].target+" "+a[0].type),b(a[0])}function d(){var a=document.querySelector("body"),b={attributes:!0,attributeOldValue:!1,characterData:!0,characterDataOldValue:!1,childList:!0,subtree:!0},d=new h(c);e("Enable MutationObserver"),d.observe(a,b)}var g=0>ab,h=a.MutationObserver||a.WebKitMutationObserver;h?g?w():d():(f("MutationObserver not supported in this browser!"),w())}function z(a){function b(a){var b=/^\d+(px)?$/i;if(b.test(a))return parseInt(a,N);var d=c.style.left,e=c.runtimeStyle.left;return c.runtimeStyle.left=c.currentStyle.left,c.style.left=a||0,a=c.style.pixelLeft,c.style.left=d,c.runtimeStyle.left=e,a}var c=document.body,d=0;return"defaultView"in document&&"getComputedStyle"in document.defaultView?(d=document.defaultView.getComputedStyle(c,null),d=null!==d?d[a]:0):d=b(c.currentStyle[a]),parseInt(d,N)}function A(a,b){for(var c=b.length,d=0,f=rb(),g=0;c>g;g++)b[g].getBoundingClientRect()[a]>d&&(d=b[g].getBoundingClientRect()[a]);return f=rb()-f,e("Parsed "+c+" HTML elements"),e("Element position calculated in "+f+"ms"),f>mb/2&&(mb=2*f,e("Event throttle increased to "+mb+"ms")),d}function B(a){return[a.bodyOffset(),a.bodyScroll(),a.documentElementOffset(),a.documentElementScroll()]}function C(a,b){function c(){return f("No tagged elements ("+b+") found on page"),W}var d=document.querySelectorAll("["+b+"]");return 0===d.length?c():A(a,d)}function D(){return document.querySelectorAll("body *")}function E(a,b,c,d){function f(){W=l,nb=m,J(W,nb,a)}function g(){function a(a,b){var c=Math.abs(a-b)<=jb;return!c}return l=void 0!==c?c:sb[Y](),m=void 0!==d?d:tb[pb](),a(W,l)||S&&a(nb,m)}function h(){return!(a in{init:1,interval:1,size:1})}function i(){return Y in fb||S&&pb in fb}function j(){e("No change in size detected")}function k(){h()&&i()?I(b):a in{interval:1}||j()}var l,m;g()||"init"===a?(G(),f()):k()}function F(a,b,c,d){function f(){a in{reset:1,resetPage:1,init:1}||e("Trigger event: "+b)}function g(){return kb&&a in T}g()?e("Trigger event cancelled: "+a):(f(),ub(a,b,c,d))}function G(){kb||(kb=!0,e("Trigger event lock on")),clearTimeout(lb),lb=setTimeout(function(){kb=!1,e("Trigger event lock off"),e("--")},U)}function H(a){W=sb[Y](),nb=tb[pb](),J(W,nb,a)}function I(a){var b=Y;Y=X,e("Reset trigger event: "+a),G(),H("reset"),Y=b}function J(a,b,c,d,f){function g(){void 0===f?f=hb:e("Message targetOrigin: "+f)}function h(){var g=a+":"+b,h=eb+":"+g+":"+c+(void 0!==d?":"+d:"");e("Sending message to host page ("+h+")"),ib.postMessage(cb+h,f)}g(),h()}function K(b){function c(){return cb===(""+b.data).substr(0,db)}function d(){$=b.data,ib=b.source,g(),V=!1,setTimeout(function(){Z=!1},U)}function h(){Z?e("Page reset ignored by init"):(e("Page size reset by host page"),H("resetPage"))}function i(){F("resizeParent","Parent window requested size check")}function j(){var a=l();_.findTarget(a)}function k(){return b.data.split("]")[1].split(":")[0]}function l(){return b.data.split(":")[1]}function m(){return"iFrameResize"in a}function n(){var a=l();e("MessageCallback called from parent: "+a),qb(JSON.parse(a)),e(" --")}function o(){return b.data.split(":")[2]in{"true":1,"false":1}}function p(){switch(k()){case"reset":h();break;case"resize":i();break;case"moveToAnchor":j();break;case"message":n();break;default:m()||o()||f("Unexpected message ("+b.data+")")}}function q(){!1===V?p():o()?d():e('Ignored message of type "'+k()+'". Received before initialization.')}c()&&q()}function L(){"loading"!==document.readyState&&a.parent.postMessage("[iFrameResizerChild]Ready","*")}var M=!0,N=10,O="",P=0,Q="",R="",S=!1,T={resize:1,click:1},U=128,V=!0,W=1,X="bodyOffset",Y=X,Z=!0,$="",_={},ab=32,bb=!1,cb="[iFrameSizer]",db=cb.length,eb="",fb={max:1,min:1,bodyScroll:1,documentElementScroll:1},gb="child",hb="*",ib=a.parent,jb=0,kb=!1,lb=null,mb=0,nb=1,ob="scroll",pb=ob,qb=function(){f("MessageCallback function not defined")},rb=Date.now||function(){return(new Date).getTime()},sb={bodyOffset:function(){return document.body.offsetHeight+z("marginTop")+z("marginBottom")},offset:function(){return sb.bodyOffset()},bodyScroll:function(){return document.body.scrollHeight},documentElementOffset:function(){return document.documentElement.offsetHeight},documentElementScroll:function(){return document.documentElement.scrollHeight},max:function(){return Math.max.apply(null,B(sb))},min:function(){return Math.min.apply(null,B(sb))},grow:function(){return sb.max()},lowestElement:function(){return Math.max(sb.bodyOffset(),A("bottom",D()))},taggedElement:function(){return C("bottom","data-iframe-height")}},tb={bodyScroll:function(){return document.body.scrollWidth},bodyOffset:function(){return document.body.offsetWidth},documentElementScroll:function(){return document.documentElement.scrollWidth},documentElementOffset:function(){return document.documentElement.offsetWidth},scroll:function(){return Math.max(tb.bodyScroll(),tb.documentElementScroll())},max:function(){return Math.max.apply(null,B(tb))},min:function(){return Math.min.apply(null,B(tb))},leftMostElement:function(){return A("left",D())},taggedElement:function(){return C("left","data-iframe-width")}},ub=c(E);b(a,"message",K),L()}(window||{});
//# sourceMappingURL=iframeResizer.contentWindow.map
(function($, window) {

	// +———————————————————————————————————————+
	//	MoveToSketch
	// +———————————————————————————————————————+

	cidocs.MoveToSketch = function( options ) {

		cidocs.Path2dSketch.call( this, options );

		this.name = "moveto";
	}

	cidocs.MoveToSketch.prototype = {

		initialize: function( options ) {
			this.superclass.initialize.call( this, options );
			this.drawInitialPath();
			this.updateSketch();
		},

		drawInitialPath: function( ) {
			
			// draw the initial path
			var path2d = new cidocs.Path2d( this.curPaper );
			path2d.moveTo( new Point( 0.0, 0.0 ) );
			this.paths.push( path2d );
			path2d.centerInCanvas( this.canvas );
		}
	};

	cidocs.MoveToSketch.extend( cidocs.Path2dSketch );

	// +———————————————————————————————————————+
	//	LineToSegmentSketch
	// +———————————————————————————————————————+

	cidocs.LineToSegmentSketch = function( options ) {

		// LineToSketch.superclass.call( this, options );
		cidocs.Path2dSketch.call( this, options );
	}

	cidocs.LineToSegmentSketch.prototype = {

		initialize: function( options ) {
			this.superclass.initialize.call( this, options );
			this.drawInitialPath();
			this.updateSketch();
		},

		drawInitialPath: function( ) {
			
			// draw the initial path
			var path2d = new cidocs.Path2d( this.curPaper );
			path2d.moveTo( new Point( 0.0, 0.0 ) );
			path2d.lineTo( new Point( 200.0, 200.0 ) );

			this.paths.push( path2d );
			path2d.centerInCanvas( this.canvas );
		}
	};

	cidocs.LineToSegmentSketch.extend( cidocs.Path2dSketch );


	// +———————————————————————————————————————+
	//	QuadToSegmentSketch
	// +———————————————————————————————————————+

	cidocs.QuadToSegmentSketch = function( options ) {

		cidocs.Path2dSketch.call( this, options );
	}

	cidocs.QuadToSegmentSketch.prototype = {

		initialize: function( options ) {

			this.superclass.initialize.call( this, options );
			this.drawInitialPath();
			this.updateSketch();
		},

		drawInitialPath: function( ) {
			
			// draw the initial path
			var path2d = new cidocs.Path2d( this.curPaper );
			path2d.moveTo( new Point( 0.0, 200.0 ) );
			path2d.quadTo( new Point( 0.0, 0.0 ), new Point( 200.0, 0.0 ) );

			this.paths.push( path2d );
			path2d.centerInCanvas( this.canvas );
		}
	};

	cidocs.QuadToSegmentSketch.extend( cidocs.Path2dSketch );


	// +———————————————————————————————————————+
	//	CubicToSegmentSketch
	// +———————————————————————————————————————+

	cidocs.CubicToSegmentSketch = function( options ) {

		cidocs.Path2dSketch.call( this, options );
	}

	cidocs.CubicToSegmentSketch.prototype = {

		initialize: function( options ) {

			this.superclass.initialize.call( this, options );
			this.drawInitialPath();
			this.updateSketch();
		},

		drawInitialPath: function( ) {
			
			// draw the initial path
			var path2d = new cidocs.Path2d( this.curPaper );
			path2d.moveTo( new Point( 0.0, 200.0 ) );
			path2d.curveTo( new Point( 100.0, 200.0 ), new Point( 100.0, 0.0 ), new Point( 200.0, 0.0 ) );

			this.paths.push( path2d );
			path2d.centerInCanvas( this.canvas );
		}
	};

	cidocs.CubicToSegmentSketch.extend( cidocs.Path2dSketch );


	// +———————————————————————————————————————+
	//	CloseSketch
	// +———————————————————————————————————————+

	cidocs.CloseSketch = function( options ) {

		cidocs.Path2dSketch.call( this, options );
		this.name = "close";
	}

	cidocs.CloseSketch.prototype = {

		initialize: function( options ) {

			this.superclass.initialize.call( this, options );
			this.drawInitialPath();
			// this.updateSketch();
		},

		drawInitialPath: function( ) {
			
			// quadTo - waves
			var curPaper = this.curPaper;		
			var path2d = new cidocs.Path2d( this.curPaper );
			path2d.moveTo( new Point( 0.0, 200.0 ) );
			path2d.lineTo( new Point( 150.0, 0.0 ) );
			path2d.lineTo( new Point( 300.0, 200.0 ) );
			path2d.close();
			this.paths.push( path2d );

			path2d.centerInCanvas( this.canvas );
		}
	};
	cidocs.CloseSketch.extend( cidocs.Path2dSketch );


	// +———————————————————————————————————————+
	//	Main App
	// +———————————————————————————————————————+

	cidocs.Part1App = function() {
		cidocs.app.call( this );
	};

	cidocs.Part1App.prototype = {

		init: function(){
			
			var self = this;

			console.log( "CODE MODULE", this.codeModule );

			// init all the sketches
			var moveToSketch			= new cidocs.MoveToSketch( { canvas:'#moveto', output:this.codeModule } );
			var lineToSegmentSketch		= new cidocs.LineToSegmentSketch( { canvas:'#linetosegment', name:'linetosegment', output:this.codeModule } );
			var quadToSegmentSketch		= new cidocs.QuadToSegmentSketch( { canvas:'#quadtosegment', name:'quadtosegment', output:this.codeModule } );
			var cubicToSegmentSketch	= new cidocs.CubicToSegmentSketch( { canvas:'#cubictosegment', name:'cubictosegment', output:this.codeModule } );
			var closeSketch				= new cidocs.CloseSketch( { canvas:'#close', output:this.codeModule } );
			this.sketches.push( moveToSketch, lineToSegmentSketch, quadToSegmentSketch, cubicToSegmentSketch, closeSketch );		


			this.superclass.init.call( this );
		}
	}

	cidocs.Part1App.extend( cidocs.app );

	$(document).ready( function(){
		
		window.app = new cidocs.Part1App();
		app.init();
		app.show( "moveto" );

	});
	

}(jQuery, window));