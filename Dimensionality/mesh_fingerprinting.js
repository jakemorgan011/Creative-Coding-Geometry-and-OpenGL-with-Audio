// Copyright (c) 2025, Jake Morgan, termite
// This code contains all the basic steps necessary to start understanding audio sculpting
// This code is free to use for any purpose.
// 
/*
                 _ _                        _       _   _             
                | (_)                      | |     | | (_)            
  __ _ _   _  __| |_  ___    ___  ___ _   _| |_ __ | |_ _ _ __   __ _ 
 / _` | | | |/ _` | |/ _ \  / __|/ __| | | | | '_ \| __| | '_ \ / _` |
| (_| | |_| | (_| | | (_) | \__ \ (__| |_| | | |_) | |_| | | | | (_| |
 \__,_|\__,_|\__,_|_|\___/  |___/\___|\__,_|_| .__/ \__|_|_| |_|\__, |
                                             | |                 __/ |
                                             |_|                |___/ 
*/
// 
// info:
// in this example I use the offlineAudioContext web API. 
// in retrospect, I would never use that again.
// I'm using their default fx "Nodes" in this code but in other examples found in my github-
// I interact with the buffer directly which proved to be extremely buggy and difficult.
//          ...shout out LLMS
// I also used an old dsp library called dsp.js...
// 
// future:
// switch to "supersonic" audio engine.
// - sonic pi web engine.
// rather than running mathmetical expressions one time as a single-pass feature extraction
// create a real-time information retrieval system.
// switch to processing on server side or just switch to full integrated application off the web.


// NOTICE:
// IF YOU ARE LOOKING FOR THE OPENGL CALLS GO TO SKETCH.JS
// it uses the p5js webgl utility.


// step 1
class obj_parser {
    static parse(text) {
        const vertices = [];
        const lines = text.split('\n');

        for (const line of lines) {
            const parts = line.trim().split(/\s+/);
            if (parts[0] === 'v') {
                vertices.push({
                    x: parseFloat(parts[1]),
                    y: parseFloat(parts[2]),
                    z: parseFloat(parts[3])
                });
            }
        }

        return vertices;
    }
}


// NOTE:
// APOLOGIES i changed my naming conventions a lot during this project...
// I'm sorry if you are searching for things and it doesn't make sense.

// get all stats HERE
class mesh_fingerprint {
    static calculate(vertices) {
        if (vertices.length === 0) return null;
        
        // just sorting to make everything fluid.
        const byX = [...vertices].sort((a, b) => a.x - b.x);
        const byY = [...vertices].sort((a, b) => a.y - b.y);
        const byZ = [...vertices].sort((a, b) => a.z - b.z);
        const byDistance = [...vertices].sort((a, b) => {
            const distA = Math.sqrt(a.x ** 2 + a.y ** 2 + a.z ** 2);
            const distB = Math.sqrt(b.x ** 2 + b.y ** 2 + b.z ** 2);
            return distA - distB;
        });

        // stats aren't normalized
        // I realize now normalizing them inside the audio processor is quite dumb...
        const stats = {
            // percentile sampling...
            x10: byX[Math.floor(vertices.length * 0.1)].x,
            x50: byX[Math.floor(vertices.length * 0.5)].x,
            x90: byX[Math.floor(vertices.length * 0.9)].x,

            y10: byY[Math.floor(vertices.length * 0.1)].y,
            y50: byY[Math.floor(vertices.length * 0.5)].y,
            y90: byY[Math.floor(vertices.length * 0.9)].y,

            z10: byZ[Math.floor(vertices.length * 0.1)].z,
            z50: byZ[Math.floor(vertices.length * 0.5)].z,
            z90: byZ[Math.floor(vertices.length * 0.9)].z,
            
            // 
            minDist: Math.sqrt(byDistance[0].x ** 2 + byDistance[0].y ** 2 + byDistance[0].z ** 2),
            medianDist: Math.sqrt(
                byDistance[Math.floor(vertices.length * 0.5)].x ** 2 +
                byDistance[Math.floor(vertices.length * 0.5)].y ** 2 +
                byDistance[Math.floor(vertices.length * 0.5)].z ** 2
            ),
            maxDist: Math.sqrt(
                byDistance[vertices.length - 1].x ** 2 +
                byDistance[vertices.length - 1].y ** 2 +
                byDistance[vertices.length - 1].z ** 2
            ),

            // spread is cool and tends to have the biggest effect when changing objects
            xSpread: byX[byX.length - 1].x - byX[0].x,
            ySpread: byY[byY.length - 1].y - byY[0].y,
            zSpread: byZ[byZ.length - 1].z - byZ[0].z,

            centerX: vertices.reduce((sum, v) => sum + v.x, 0) / vertices.length,
            centerY: vertices.reduce((sum, v) => sum + v.y, 0) / vertices.length,
            centerZ: vertices.reduce((sum, v) => sum + v.z, 0) / vertices.length,

            variance: this.calculate_variance(vertices)
        };

        return stats;
    }
    
    // not a great mathmetitian so excuse how primitive this is.
    static calculate_variance(vertices) {
        const mean = {
            x: vertices.reduce((sum, v) => sum + v.x, 0) / vertices.length,
            y: vertices.reduce((sum, v) => sum + v.y, 0) / vertices.length,
            z: vertices.reduce((sum, v) => sum + v.z, 0) / vertices.length
        };

        const variance = vertices.reduce((sum, v) => {
            return sum +
                Math.pow(v.x - mean.x, 2) +
                Math.pow(v.y - mean.y, 2) +
                Math.pow(v.z - mean.z, 2);
        }, 0) / vertices.length;

        return variance;
    }
    
    // yea okay definitely don't normalize inside a mapping function...
    // DO ALL YOUR NORMALIZING WITHIN THE STATS OBJECT IT MAKES IT SO MUCH EASIER AND UNDERSTANDABLE.
    static mapToAudioParams(stats) {
        const normalize = (value, min, max) => {
            return Math.max(0, Math.min(1, (value - min) / (max - min)));
        };

        return {
            // regular parameterization here nothing crazy.
            reverbSize: normalize(stats.medianDist, 0, 10),
            reverbDamping: normalize(stats.variance, 0, 5),
            reverbWetness: normalize(stats.ySpread, 0, 5) * 0.5,

            delayTime: normalize(stats.zSpread, 0, 5) * 0.5, // 0-500ms
            delayFeedback: normalize(Math.abs(stats.centerZ), 0, 3) * 0.7,

            distortionAmount: normalize(stats.variance, 0, 10) * 50,

            // fairly lazy parameterization done in this section I would recomend playing with this a little more than I did.
            expansionThreshold: -40 + normalize(stats.centerX, -5, 5) * 30, // -40dB to -10dB
            expansionRatio: 1 + normalize(Math.abs(stats.centerX), 0, 5) * 4, // 1:1 to 5:1

            chorusDepth: normalize(stats.x90 - stats.x10, 0, 5),
            chorusRate: 0.5 + normalize(stats.minDist, 0, 2) * 4,

            flangingDepth: normalize(stats.variance, 0, 10) * 0.01, // 0-10ms delay

            combFilterFreq: 100 + normalize(stats.variance, 0, 10) * 900, // 100-1000 Hz

            //needs harsh stats to activate. aka crazy shapes. high spread high variance.
            feedbackIntensity: normalize(stats.ySpread, 0, 5) * normalize(stats.variance, 0, 10), // 0-1
            feedbackChaos: normalize(stats.xSpread + stats.zSpread, 0, 10) // How chaotic the network is
        };
    }
}


// IGNORE THIS IF YOU JUST WANT TO SEE HOW ITS DONE.
// 
// this is my 'spectral_mashing' effect that got axed during the course of this project.
// I was having some trouble interacting with the buffer and not breaking playback with OfflineAudioContext
// so it was ultimately scrapped from the production version.
// My math wasn't very strong when I wrote this so i couldn't do much without a environment like maxmsp

// import { FFT } from 'src/lib/dsp.js';

async function spectral_mash(inputBuffer, meshStats) {
    const sampleRate = inputBuffer.sampleRate;
    const input = inputBuffer.getChannelData(0);
    const output = new Float32Array(input.length);

    const fftSize = 4096;
    const fft = new FFT(fftSize, sampleRate);

    for (let i = 0; i < input.length - fftSize; i += fftSize / 2) {
        // i forgot slice existed.
        const chunk = input.slice(i, i + fftSize);

        fft.forward(chunk);

        const real = fft.real;
        const imag = fft.imag;
        // mesh calc
        // the actuall PROCESSING HAPPENS HERE. PUT ALL YOUR SPECTRAL DSP IN HERE.
        // i literally know ZERO spectral dsp other than vocoding and de-harm
        // i guess i'll just make stuff up then.
        for (let bin = 0; bin < real.length; bin++) {
            const freq = bin * sampleRate / fftSize;
            const gain = 1 + meshStats.variance * (freq / 1000);
        }

        const processed = fft.inverse(real, imag);

        for (let j = 0; j < processed.length; j++) {
            if (i + j < output.length) {
                output[i + j] += processed[j] * 0.5;
            }
        }
    }
    return output;
}


// here we go.
// this is where stuff happens
class audio_processor {
    constructor() {
        this.audioContext = null;
        this.sourceBuffer = null;
        this.processedBuffer = null;
        this.isPlaying = false;
        this.currentSource = null;
    }
    // basic loaders. nothing important here.
    async init() {
        this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
    }

    async loadAudioFile(file) {
        const arrayBuffer = await file.arrayBuffer();
        this.sourceBuffer = await this.audioContext.decodeAudioData(arrayBuffer);
        return this.sourceBuffer;
    }
    
    //
    // basically all this does is assign the stats object we made to all of our offlineAudioContext `nodes`
    // i personally don't mind the node system but would've liked to be able to create custom nodes easier.
    async process_with_parameters(params) {
        if (!this.sourceBuffer) {
            throw new Error('No audio loaded');
        }

        const offlineContext = new OfflineAudioContext(
            this.sourceBuffer.numberOfChannels,
            this.sourceBuffer.length,
            this.sourceBuffer.sampleRate
        );

        const source = offlineContext.createBufferSource();
        source.buffer = this.sourceBuffer;

        let currentNode = source;

        if (params.distortionAmount > 0) {
            const distortion = offlineContext.createWaveShaper();
            distortion.curve = this.makeDistortionCurve(params.distortionAmount);
            distortion.oversample = '4x';
            currentNode.connect(distortion);
            currentNode = distortion;
        }

        const delay = offlineContext.createDelay(1.0);
        const delayGain = offlineContext.createGain();
        const delayFeedback = offlineContext.createGain();
        const delayWet = offlineContext.createGain();

        delay.delayTime.value = params.delayTime;
        delayFeedback.gain.value = params.delayFeedback;
        delayWet.gain.value = 0.3;
        // 
        // here you have to connect all nodes to our buffer/audioContext.
        // this gets complicated when you start doing custom fx
        // don't use offlineAudioContext...
        currentNode.connect(delay);
        delay.connect(delayGain);
        delayGain.connect(delayFeedback);
        delayFeedback.connect(delay);
        delayGain.connect(delayWet);

        const convolver = offlineContext.createConvolver();
        convolver.buffer = this.createReverbImpulse(
            offlineContext.sampleRate,
            params.reverbSize * 4,
            params.reverbDamping
        );

        const reverbWet = offlineContext.createGain();
        reverbWet.gain.value = params.reverbWetness;

        currentNode.connect(convolver);
        convolver.connect(reverbWet);

        const dryGain = offlineContext.createGain();
        dryGain.gain.value = 0.7;
        currentNode.connect(dryGain);

        // put it all back into one buffer.
        const finalMix = offlineContext.createGain();
        dryGain.connect(finalMix);
        delayWet.connect(finalMix);
        reverbWet.connect(finalMix);

        finalMix.connect(offlineContext.destination);


        // render at first sample.
        source.start(0);

        let renderedBuffer = await offlineContext.startRendering();


        // this is where the custom stuff comes in but its alright
        renderedBuffer = this.applyDownwardExpansion(
            renderedBuffer,
            params.expansionThreshold,
            params.expansionRatio
        );

        this.processedBuffer = renderedBuffer;
        return this.processedBuffer;
    }

    applyDownwardExpansion(buffer, thresholdDb, ratio) {
        const numChannels = buffer.numberOfChannels;
        const length = buffer.length;
        const sampleRate = buffer.sampleRate;

        // copy buffer
        const expandedBuffer = this.audioContext.createBuffer(numChannels, length, sampleRate);

        const thresholdLinear = Math.pow(10, thresholdDb / 20);
        const attackSamples = Math.floor(0.001 * sampleRate); // 1ms attack
        const releaseSamples = Math.floor(0.05 * sampleRate); // 50ms release

        // buffer process here.
        // as things get more custom and complicated,
        // you are guaranteed to introduce more time complexity...
        // solving this problem is my current fun debacle. :3
        for (let channel = 0; channel < numChannels; channel++) {
            const inputData = buffer.getChannelData(channel);
            const outputData = expandedBuffer.getChannelData(channel);
            let envelope = 1.0;
            
            // downward expansiion here. loud sounds louder. quiet sounds quieter.
            for (let i = 0; i < length; i++) {
                const inputSample = inputData[i];
                const inputLevel = Math.abs(inputSample);

                let targetGain = 1.0;
                if (inputLevel < thresholdLinear && inputLevel > 0) {
                    const levelRatio = inputLevel / thresholdLinear;
                    targetGain = Math.pow(levelRatio, ratio - 1);
                }

                if (targetGain < envelope) {
                    envelope += (targetGain - envelope) / attackSamples;
                } else {
                    envelope += (targetGain - envelope) / releaseSamples;
                }
                outputData[i] = inputSample * envelope;
            }
        }

        return expandedBuffer;
    }
    
    // a random curve I took from an old stanford paper lol.
    makeDistortionCurve(amount) {
        const samples = 44100;
        const curve = new Float32Array(samples);
        const deg = Math.PI / 180;

        for (let i = 0; i < samples; i++) {
            const x = (i * 2) / samples - 1;
            curve[i] = ((3 + amount) * x * 20 * deg) / (Math.PI + amount * Math.abs(x));
        }

        return curve;
    }

    // same thing for this. 
    createReverbImpulse(sampleRate, duration, decay) {
        const length = sampleRate * duration;
        const impulse = new Float32Array(length);

        for (let i = 0; i < length; i++) {
            impulse[i] = (Math.random() * 2 - 1) * Math.pow(1 - i / length, decay);
        }

        return this.audioContext.createBuffer(1, length, sampleRate);
    }
    
    // playback time. has a lot of weird issues if you start creating custom effects.
    play(buffer, flangingDepth = 0, combFilterFreq = 0, feedbackIntensity = 0, feedbackChaos = 0) {
        if (this.isPlaying) this.stop();

        this.currentSource = this.audioContext.createBufferSource();
        this.currentSource.buffer = buffer || this.processedBuffer;

        let currentNode = this.currentSource;

        // add flanging effect if
        if (flangingDepth > 0) {
            const flangeDelay = this.audioContext.createDelay(0.02); // Max 20ms
            const flangeGain = this.audioContext.createGain();
            const flangeLFO = this.audioContext.createOscillator();
            const lfoGain = this.audioContext.createGain();
            const feedbackGain = this.audioContext.createGain();
            const wetGain = this.audioContext.createGain();
            const dryGain = this.audioContext.createGain();

            flangeLFO.type = 'sine';
            flangeLFO.frequency.value = 0.5; // 0.5 Hz sweep
            lfoGain.gain.value = flangingDepth; // Delay modulation depth

            flangeLFO.connect(lfoGain);
            lfoGain.connect(flangeDelay.delayTime);

            flangeDelay.delayTime.value = flangingDepth;

            feedbackGain.gain.value = 0.7;
            wetGain.gain.value = 0.5;
            dryGain.gain.value = 0.7;

            currentNode.connect(flangeDelay);
            flangeDelay.connect(feedbackGain);
            feedbackGain.connect(flangeDelay); // Feedback loop
            flangeDelay.connect(wetGain);

            currentNode.connect(dryGain);

            const flangeMix = this.audioContext.createGain();
            dryGain.connect(flangeMix);
            wetGain.connect(flangeMix);

            currentNode = flangeMix;
            flangeLFO.start(0);

            this.effectLFOs = [flangeLFO];
        }

        if (combFilterFreq > 0) {
            const combDelay = this.audioContext.createDelay(1.0);
            const combGain = this.audioContext.createGain();
            const combMix = this.audioContext.createGain();

            combDelay.delayTime.value = 1.0 / combFilterFreq;
            combGain.gain.value = 0.7; // Feedforward gain

            currentNode.connect(combDelay);
            combDelay.connect(combGain);

            currentNode.connect(combMix);
            combGain.connect(combMix);

            currentNode = combMix;
        }

        // CHAOTIC FEEDBACK NETWORK  can completely override the sound
        if (feedbackIntensity > 0.3) { // Only activate with harsh stats
            // Create multiple delay lines with cross-feedback
            const numNodes = 4;
            const delays = [];
            const filters = [];
            const gains = [];
            const lfos = [];

            // Input gain - reduce original signal as feedback increases
            const inputGain = this.audioContext.createGain();
            inputGain.gain.value = Math.max(0.1, 1 - feedbackIntensity);
            currentNode.connect(inputGain);

            // the 'offlineAudioContext' way of doing feedback networks. it's kinda goofy.
            for (let i = 0; i < numNodes; i++) {
                const delay = this.audioContext.createDelay(1.0);
                const filter = this.audioContext.createBiquadFilter();
                const gain = this.audioContext.createGain();
                const lfo = this.audioContext.createOscillator();
                const lfoGain = this.audioContext.createGain();

                const baseDelay = 0.01 + (i / numNodes) * 0.29;
                delay.delayTime.value = baseDelay;

                lfo.type = ['sine', 'triangle', 'square', 'sawtooth'][i % 4];
                lfo.frequency.value = 0.2 + feedbackChaos * (i + 1) * 0.5;
                lfoGain.gain.value = 0.005 + feedbackChaos * 0.02;
                lfo.connect(lfoGain);
                lfoGain.connect(delay.delayTime);

                filter.type = ['lowpass', 'highpass', 'bandpass', 'notch'][i % 4];
                filter.frequency.value = 200 + i * 500 + feedbackChaos * 1000;
                filter.Q.value = 1 + feedbackChaos * 10;

                gain.gain.value = 0.3 + feedbackIntensity * 0.7;

                delays.push(delay);
                filters.push(filter);
                gains.push(gain);
                lfos.push(lfo);

                lfo.start(0);
            }


            // connect all the 'nodes'
            for (let i = 0; i < numNodes; i++) {
                inputGain.connect(delays[i]);
            }

            for (let i = 0; i < numNodes; i++) {
                delays[i].connect(filters[i]);
                filters[i].connect(gains[i]);

                for (let j = 0; j < numNodes; j++) {
                    if (i !== j) {
                        const crossGain = this.audioContext.createGain();
                        crossGain.gain.value = (0.1 + feedbackIntensity * 0.3) / numNodes;
                        gains[i].connect(crossGain);
                        crossGain.connect(delays[j]);
                    }
                }
            }

            // mix
            const feedbackMix = this.audioContext.createGain();
            const feedbackWetGain = this.audioContext.createGain();
            feedbackWetGain.gain.value = feedbackIntensity * 2; // Can overdrive the mix

            for (let i = 0; i < numNodes; i++) {
                gains[i].connect(feedbackWetGain);
            }

            feedbackWetGain.connect(feedbackMix);
            inputGain.connect(feedbackMix);

            // limiter 
            const clipper = this.audioContext.createWaveShaper();
            clipper.curve = this.makeDistortionCurve(20);
            feedbackMix.connect(clipper);

            currentNode = clipper;

            if (!this.effectLFOs) this.effectLFOs = [];
            this.effectLFOs.push(...lfos);
        }

        currentNode.connect(this.audioContext.destination);

        this.currentSource.start(0);
        this.isPlaying = true;

        this.currentSource.onended = () => {
            this.isPlaying = false;
            if (this.effectLFOs) {
                this.effectLFOs.forEach(lfo => {
                    try { lfo.stop(); } catch (e) { }
                });
                this.effectLFOs = null;
            }
        };
    }

    stop() {
        if (this.currentSource) {
            this.currentSource.stop();
            this.currentSource = null;
            this.isPlaying = false;
        }
        if (this.effectLFOs) {
            this.effectLFOs.forEach(lfo => {
                try { lfo.stop(); } catch (e) { }
            });
            this.effectLFOs = null;
        }
    }



    downloadProcessedAudio(filename = 'processed.wav') {
        if (!this.processedBuffer) {
            throw new Error('No processed audio available');
        }

        const wav = this.audioBufferToWav(this.processedBuffer);
        const blob = new Blob([wav], { type: 'audio/wav' });
        const url = URL.createObjectURL(blob);

        const a = document.createElement('a');
        a.href = url;
        a.download = filename;
        a.click();

        URL.revokeObjectURL(url);
    }

    audioBufferToWav(buffer) {
        const numChannels = buffer.numberOfChannels;
        const sampleRate = buffer.sampleRate;
        const format = 1; // PCM
        const bitDepth = 16;

        const bytesPerSample = bitDepth / 8;
        const blockAlign = numChannels * bytesPerSample;

        const data = [];
        for (let i = 0; i < buffer.numberOfChannels; i++) {
            data.push(buffer.getChannelData(i));
        }

        const length = data[0].length;
        const arrayBuffer = new ArrayBuffer(44 + length * blockAlign);
        const view = new DataView(arrayBuffer);

        const writeString = (offset, string) => {
            for (let i = 0; i < string.length; i++) {
                view.setUint8(offset + i, string.charCodeAt(i));
            }
        };

        writeString(0, 'RIFF');
        view.setUint32(4, 36 + length * blockAlign, true);
        writeString(8, 'WAVE');
        writeString(12, 'fmt ');
        view.setUint32(16, 16, true);
        view.setUint16(20, format, true);
        view.setUint16(22, numChannels, true);
        view.setUint32(24, sampleRate, true);
        view.setUint32(28, sampleRate * blockAlign, true);
        view.setUint16(32, blockAlign, true);
        view.setUint16(34, bitDepth, true);
        writeString(36, 'data');
        view.setUint32(40, length * blockAlign, true);

        let offset = 44;
        for (let i = 0; i < length; i++) {
            for (let channel = 0; channel < numChannels; channel++) {
                const sample = Math.max(-1, Math.min(1, data[channel][i]));
                view.setInt16(offset, sample * 0x7FFF, true);
                offset += 2;
            }
        }

        return arrayBuffer;
    }
}

// from here its all html interaction for GUI and UX

// ui control
const audioProcessor = new audio_processor();
let meshFingerprint = null;
let audioParams = null;

// start processing when interact`
document.addEventListener('click', () => {
    if (!audioProcessor.audioContext) {
        audioProcessor.init();
    }
}, { once: true });

document.getElementById('obj-input').addEventListener('change', async (e) => {
    const file = e.target.files[0];
    if (file) {
        const text = await file.text();
        const vertices = obj_parser.parse(text);

        document.getElementById('status-text').textContent = `Loaded ${vertices.length} vertices`;

        meshFingerprint = mesh_fingerprint.calculate(vertices);
        audioParams = mesh_fingerprint.mapToAudioParams(meshFingerprint);

        const paramsDisplay = document.getElementById('params-display');
        const paramsList = document.getElementById('params-list');
        paramsDisplay.style.display = 'block';

        paramsList.innerHTML = Object.entries(audioParams)
            .map(([key, value]) => `<p>${key}: ${value.toFixed(3)}</p>`)
            .join('');

        if (audioProcessor.sourceBuffer) {
            document.getElementById('process-btn').disabled = false;
        }
    }
});

document.getElementById('wav-input').addEventListener('change', async (e) => {
    const file = e.target.files[0];
    if (file) {
        await audioProcessor.init();
        await audioProcessor.loadAudioFile(file);

        document.getElementById('status-text').textContent = 'Audio loaded';

        if (audioParams) {
            document.getElementById('process-btn').disabled = false;
        }
    }
});

document.getElementById('process-btn').addEventListener('click', async () => {
    if (audioParams && audioProcessor.sourceBuffer) {
        document.getElementById('status-text').textContent = 'Processing...';

        try {
            await audioProcessor.process_with_parameters(audioParams);

            document.getElementById('status-text').textContent = 'Processing complete!';
            document.getElementById('play-btn').disabled = false;
            document.getElementById('download-btn').disabled = false;
        } catch (error) {
            document.getElementById('status-text').textContent = `Error: ${error.message}`;
        }
    }
});

document.getElementById('play-btn').addEventListener('click', () => {
    const flangingDepth = audioParams ? audioParams.flangingDepth : 0;
    const combFilterFreq = audioParams ? audioParams.combFilterFreq : 0;
    const feedbackIntensity = audioParams ? audioParams.feedbackIntensity : 0;
    const feedbackChaos = audioParams ? audioParams.feedbackChaos : 0;
    audioProcessor.play(null, flangingDepth, combFilterFreq, feedbackIntensity, feedbackChaos);
    document.getElementById('stop-btn').disabled = false;
});

document.getElementById('stop-btn').addEventListener('click', () => {
    audioProcessor.stop();
    document.getElementById('stop-btn').disabled = true;
});

document.getElementById('download-btn').addEventListener('click', () => {
    audioProcessor.downloadProcessedAudio('dimensionality-processed.wav');
});
