/*
	Code-It-Yourself: Remote Home Energy Monitor
	"C'mon Mum, Keep going!" - javidx9

	Video: https://youtu.be/NJXEzL3peIA

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018 - 2025 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Video:
	~~~~~~
	https://youtu.be/NJXEzL3peIA

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	X:			https://www.x.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, (c) OneLoneCoder 2025
*/

graph =  [];
tick = 0;

function OnLoaded() {
    const ctx = document.getElementById('EnergyUsageChart')

    c = new Chart(ctx, {
    type: 'scatter',
    data: {
    datasets:[{
        label: "Watts",
        data: graph,
        showLine: true,
        tension: 0.4,
        fill: 'origin'
    }]
    },
    options:{
        scales: {
            x: {
                min: 10,
                max: 50,
            }

    }}
    });

    c.options.animation = false; // disables all animations
    c.options.animations.colors = false; // disables animation defined by the collection of 'colors' properties
    c.options.animations.x = false; // disables animation defined by the 'x' property
    c.options.transitions.active.animation.duration = 0; // disables the animation for 'active' mode

    socket = new WebSocket('wss://onelonecoder.com:12345');

    socket.onopen = function(event) {
        connected = true;
    };

    socket.onmessage = function(event) {
        const obj = JSON.parse(event.data);
        console.log(obj);

        tick = Math.max(tick, obj.x);
        
        if(graph.length > 100)
            graph.shift();

        // Convert inter-pulse time into seconds
        pulse_length = parseFloat(obj.y) / 1000.0;
        watts = 1000.0 *(3600.0 / (3200.0) / pulse_length);

        if(watts < 40000.0)
        {
            graph.push({x:obj.x,y:watts});
            document.getElementById("watts_now").innerHTML = "Watts: " + watts.toFixed(2);
            c.options.scales.x.min = tick - 99;
            c.options.scales.x.max = tick;
            c.update();
        }     
    };

    socket.onclose = function(event) {
        connected = false;
    };
}