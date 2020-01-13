var websock;
var password = false;
var maxNetworks;
var host;
var port;

var matrixWidth = 8;
var matrixHeight = 8;
var matrixStep;
var drawingColor = "#ffffff";

var timer;

// http://www.the-art-of-web.com/javascript/validate-password/
function checkPassword(str) {
    // at least one number, one lowercase and one uppercase letter
    // at least eight characters that are letters, numbers or the underscore
    var re = /^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])\w{8,}$/;
    return re.test(str);
}

function validateForm(form) {

    // password
    var adminPass1 = $("input[name='adminPass1']", form).val();
    if (adminPass1.length > 0 && !checkPassword(adminPass1)) {
        alert("The password you have entered is not valid, it must have at least 8 characters, 1 lower and 1 uppercase and 1 number!");
        return false;
    }

    var adminPass2 = $("input[name='adminPass2']", form).val();
    if (adminPass1 != adminPass2) {
        alert("Passwords are different!");
        return false;
    }

    return true;

}

function updateTime() {
    var d = new Date();
    var value =
        zeroPad(d.getDate(), 2) + "/" +
        zeroPad(d.getMonth() + 1, 2) + "/" +
        zeroPad(d.getFullYear(), 4) + " " +
        zeroPad(d.getHours(), 2) + ":" +
        zeroPad(d.getMinutes(), 2) + ":" +
        zeroPad(d.getSeconds(), 2);
    $("input[name='timeNow']").val(value);
}

function doUpdate() {
    var form = $("#formSave");
    if (validateForm(form)) {
        var data = form.serializeArray();
        delete(data['filename']);
        websock.send(JSON.stringify({'config': data}));
    }
    return false;
}

function doUpgrade() {

    var contents = $("input[name='upgrade']")[0].files[0];
    if (typeof contents == 'undefined') {
        alert("First you have to select a file from your computer.");
        return false;
    }
    var filename = $("input[name='upgrade']").val().split('\\').pop();

    var data = new FormData();
    data.append('upgrade', contents, filename);

    $.ajax({

        // Your server script to process the upload
        url: 'http://' + host + ':' + port + '/upgrade',
        type: 'POST',

        // Form data
        data: data,

        // Tell jQuery not to process data or worry about content-type
        // You *must* include these options!
        cache: false,
        contentType: false,
        processData: false,

        success: function(data, text) {
            $("#upgrade-progress").hide();
            if (data == 'OK') {
                alert("Firmware image uploaded, board rebooting. This page will be refreshed in 5 seconds.");
                setTimeout(function() {
                    window.location = "/";
                }, 5000);
            } else {
                alert("There was an error trying to upload the new image, please try again.");
            }
        },

        // Custom XMLHttpRequest
        xhr: function() {
            $("#upgrade-progress").show();
            var myXhr = $.ajaxSettings.xhr();
            if (myXhr.upload) {
                // For handling the progress of the upload
                myXhr.upload.addEventListener('progress', function(e) {
                    if (e.lengthComputable) {
                        $('progress').attr({ value: e.loaded, max: e.total });
                    }
                } , false);
            }
            return myXhr;
        },

    });

    return false;

}

function doUpdatePassword() {
    var form = $("#formPassword");
    if (validateForm(form)) {
        var data = form.serializeArray();
        websock.send(JSON.stringify({'config': data}));
    }
    return false;
}

function doReset() {
    var response = window.confirm("Are you sure you want to reset the device?");
    if (response == false) return false;
    websock.send(JSON.stringify({'action': 'reset'}));
    return false;
}

function doReconnect() {
    var response = window.confirm("Are you sure you want to disconnect from the current WIFI network?");
    if (response == false) return false;
    websock.send(JSON.stringify({'action': 'reconnect'}));
    return false;
}

function backupSettings() {
    document.getElementById('downloader').src = 'http://' + host + ':' + port + '/config';
    return false;
}

function onFileUpload(event) {

    var inputFiles = this.files;
    if (inputFiles == undefined || inputFiles.length == 0) return false;
    var inputFile = inputFiles[0];
    this.value = "";

    var response = window.confirm("Previous settings will be overwritten. Are you sure you want to restore this settings?");
    if (response == false) return false;

    var reader = new FileReader();
    reader.onload = function(e) {
        var data = getJson(e.target.result);
        if (data) {
            websock.send(JSON.stringify({'action': 'restore', 'data': data}));
        } else {
            alert("The file is not a configuration backup or is corrupted");
        }
    };
    reader.readAsText(inputFile);

    return false;

}

function restoreSettings() {
    if (typeof window.FileReader !== 'function') {
        alert("The file API isn't supported on this browser yet.");
    } else {
        $("#uploader").click();
    }
    return false;
}

function setPixel(x, y, color) {

    // draw square
    var context = $('#matrix').get(0).getContext("2d");
    context.fillStyle = color;
    context.fillRect(
        x * matrixStep + 1,
        y * matrixStep + 1,
        matrixStep - 1,
        matrixStep - 1);

}

function clearCanvas() {
    var empty = "#000000";
    for (x=0; x<matrixWidth; x++) {
        for (y=0; y<matrixHeight; y++) {
            setPixel(x, y, empty);
        }
    }
}

function doCanvas() {
    clearCanvas();
    websock.send(JSON.stringify({'action': 'driver', 'data' : {'name': 'canvas'}}));
}

function updateCanvas() {

    var width = 512;
    var height = width;
    matrixStep = width / matrixWidth;

    var canvas = $('#matrix')
        .attr({width: width + 10, height: height + 10})
        .on('mousedown', function(e) {

            // get coordinates
            var x = parseInt(e.offsetX / matrixStep);
            var y = parseInt(e.offsetY / matrixStep);

            // draw square
            setPixel(x, y, drawingColor);

            // send value
            websock.send(JSON.stringify({'action': 'color', 'data' : { 'x': x, 'y': y, 'color': drawingColor}}));

        });

    clearCanvas();

}

function showPanel() {
    $(".panel").hide();
    $("#" + $(this).attr("data")).show();
    if ($("#layout").hasClass('active')) toggleMenu();
};

function toggleMenu() {
    $("#layout").toggleClass('active');
    $("#menu").toggleClass('active');
    $("#menuLink").toggleClass('active');
}

function delNetwork() {
    var parent = $(this).parents(".pure-g");
    $(parent).remove();
}

function moreNetwork() {
    var parent = $(this).parents(".pure-g");
    $("div.more", parent).toggle();
}

function addNetwork() {

    var numNetworks = $("#networks > div").length;
    if (numNetworks >= maxNetworks) {
        alert("Max number of networks reached");
        return;
    }

    var tabindex = 200 + numNetworks * 10;
    var template = $("#networkTemplate").children();
    var line = $(template).clone();
    $(line).find("input").each(function() {
        $(this).attr("tabindex", tabindex++);
    });
    $(line).find(".button-del-network").on('click', delNetwork);
    $(line).find(".button-more-network").on('click', moreNetwork);
    line.appendTo("#networks");

    return line;

}

function forgetCredentials() {
    $.ajax({
        'method': 'GET',
        'url': '/',
        'async': false,
        'username': "logmeout",
        'password': "123456",
        'headers': { "Authorization": "Basic xxx" }
    }).done(function(data) {
        return false;
        // If we don't get an error, we actually got an error as we expect an 401!
    }).fail(function(){
        // We expect to get an 401 Unauthorized error! In this case we are successfully
        // logged out and we redirect the user.
        return true;
    });
}

function processData(data) {

    // title
    if ("app" in data) {
        var title = data.app;
		if ("version" in data) {
			title = title + " " + data.version;
		}
        $(".pure-menu-heading").html(title);
        if ("hostname" in data) {
            title = data.hostname + " - " + title;
        }
        document.title = title;
    }

    Object.keys(data).forEach(function(key) {

        // Web Modes
        if (key == "webMode") {
            password = data.webMode == 1;
            $("#layout").toggle(data.webMode == 0);
            $("#password").toggle(data.webMode == 1);
            $("#credentials").hide();
        }

        // Actions
        if (key == "action") {

            if (data.action == "reload") {
                if (password) forgetCredentials();
                setTimeout(function() {
                    window.location = "/";
                }, 1000);
            }

            return;

        }

        if (key == "maxNetworks") {
            maxNetworks = parseInt(data.maxNetworks);
            return;
        }

        if (key == "matrixWidth") {
            matrixWidth = parseInt(data.matrixWidth);
            return;
        }
        if (key == "matrixHeight") {
            matrixHeight = parseInt(data.matrixHeight);
            return;
        }

        // Wifi
        if (key == "wifi") {

            var networks = data.wifi;

            for (var i in networks) {

                // add a new row
                var line = addNetwork();

                // fill in the blanks
                var wifi = data.wifi[i];
                Object.keys(wifi).forEach(function(key) {
                    var element = $("input[name=" + key + "]", line);
                    if (element.length) element.val(wifi[key]);
                });

            }

            return;

        }

        // Messages
        if (key == "message") {
            window.alert(data.message);
            return;
        }

        // Enable options
        if (key.endsWith("Visible")) {
            var module = key.slice(0,-7);
            $(".module-" + module).show();
            return;
        }

        // Pre-process
        if (key == "network") {
            data.network = data.network.toUpperCase();
        }
        if (key == "mqttStatus") {
            data.mqttStatus = data.mqttStatus ? "CONNECTED" : "NOT CONNECTED";
        }

        // Look for INPUTs
        var element = $("input[name=" + key + "]");
        if (element.length > 0) {
            if (element.attr('type') == 'checkbox') {
                element.prop("checked", data[key])
            } else if (element.attr('type') == 'radio') {
                element.val([data[key]]);
            } else {
                element.val(data[key]);
            }
            return;
        }

        // Look for SELECTs
        var element = $("select[name=" + key + "]");
        if (element.length > 0) {
            element.val(data[key]);
            return;
        }

    });

    // Auto generate an APIKey if none defined yet
    if ($("input[name='apiKey']").val() == "") {
        doGenerateAPIKey();
    }

    updateCanvas();

}

function getJson(str) {
    try {
        return JSON.parse(str);
    } catch (e) {
        return false;
    }
}

function connect(h, p) {

    if (typeof h === 'undefined') {
        h = window.location.hostname;
    }
    if (typeof p === 'undefined') {
        p = location.port;
    }
    host = h;
    port = p;

    if (websock) websock.close();
    websock = new WebSocket('ws://' + host + ':' + port + '/ws');
    websock.onopen = function(evt) {
        console.log("Connected");
    };
    websock.onclose = function(evt) {
        console.log("Disconnected");
    };
    websock.onerror = function(evt) {
        console.log("Error: ", evt);
    };
    websock.onmessage = function(evt) {
        var data = getJson(evt.data);
        if (data) processData(data);
    };
}

function init() {

    $("#menuLink").on('click', toggleMenu);
    $(".button-update").on('click', doUpdate);
    $(".button-update-password").on('click', doUpdatePassword);
    $(".button-reset").on('click', doReset);
    $(".button-reconnect").on('click', doReconnect);
    //$(".button-settings-backup").on('click', backupSettings);
    //$(".button-settings-restore").on('click', restoreSettings);
    $('#uploader').on('change', onFileUpload);
    $(".button-upgrade").on('click', doUpgrade);
    $(".button-clear-canvas").on('click', doCanvas);
    $(".button-upgrade-browse").on('click', function() {
        $("input[name='upgrade']")[0].click();
        return false;
    });
    $("input[name='upgrade']").change(function (){
        var fileName = $(this).val();
        $("input[name='filename']").val(fileName.replace(/^.*[\\\/]/, ''));
    });
    $('progress').attr({ value: 0, max: 100 });
    $(".pure-menu-link").on('click', showPanel);
    $(".button-add-network").on('click', function() {
        $("div.more", addNetwork()).toggle();
    });
    $('input[name="color"]').wheelColorPicker({
        sliders: 'wsvp'
    }).on('sliderup', function() {
        drawingColor = $(this).wheelColorPicker('getValue', 'css');
    });

    var host = window.location.hostname;
    var port = location.port;

    $.ajax({
        'method': 'GET',
        'url': 'http://' + host + ':' + port + '/auth'
    }).done(function(data) {
        connect();
    }).fail(function(){
        $("#credentials").show();
    });

    timer = setInterval(updateTime, 1000);

}

$(init);
