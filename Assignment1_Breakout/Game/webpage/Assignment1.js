// adapted from PM1.js
var pm3 = {
    currentTab: null,
    lastBtn   : null
}

// Toggle selected tab
pm3.selectTab = function (contentID) {
    pm3.currentTab.classList.remove('selected');
    pm3.currentTab.classList.add('unselected');
    pm3.currentTab = document.getElementById(contentID);
    pm3.currentTab.classList.remove('unselected');
    pm3.currentTab.classList.add('selected');
}

window.addEventListener('load', function () {
    var tabs = document.getElementsByClassName("tab");
    var content = document.getElementsByClassName("tabcontent");

    // set "Content" as the lastBtn and highlighted
    pm3.lastBtn = tabs[0];
    pm3.lastBtn.style.backgroundColor = "#111";
    // Add functionality to change content on click
    for (var i = 0; i < tabs.length; i++) {
	tabs[i].addEventListener('click', function (evt) {
        // reset last btn colors
        pm3.lastBtn.style.background = "#333";
        // highlight clicked btn
	    var tab = document.getElementById(evt.target.id);
        tab.style.backgroundColor = "#111";
        // update lastBtn
        pm3.lastBtn = tab;
	    return pm3.selectTab(tab.getAttribute("associated-content"));
	});
    tabs[i].addEventListener('mouseover', function (evt) {
        var tab = document.getElementById(evt.target.id);
        tab.style.backgroundColor = "#111";
    });
    // mouseleave only works when the leaving button is not selected (clicked)
    tabs[i].addEventListener('mouseleave', function (evt) {
        var tab = document.getElementById(evt.target.id);
        if(evt.target != pm3.lastBtn){
            tab.style.backgroundColor = "#333";
        }
    });
    }

    // Set only first content to shown
    for (var i = 1; i < content.length; i++) {
	content[i].classList.add('unselected');
    }
    pm3.currentTab = content[0];
    content[0].classList.add('selected');

    var acc = document.getElementsByClassName("accordion");
    var i;
    
    for (i = 0; i < acc.length; i++) {
        acc[i].addEventListener("click", function() {
            this.classList.toggle("active");
            var panel = this.nextElementSibling;
            if (panel.style.display === "block") {
                panel.style.display = "none";
            } else {
                panel.style.display = "block";
            }
        });
    }
});
