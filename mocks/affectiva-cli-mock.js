#! /usr/local/bin/node

'use strict';

var sample_data = [{
    "date and time" : "1-19-2016 14:9:33",
    "timestamp" : 0.000000,
    "measurements" : {
        "yaw" : -0.063200,
        "roll" : 7.547718,
        "interoculardist" : 243.320160
    },
    "expressions" : {
        "smile" : 0,
        "innerBrowRaise" : 0,
        "browRaise" : 0,
        "browFurrow" : 0,
        "noseWrinkle" : 0,
        "upperLipRaise" : 0,
        "lipCornerDepressor" : 0,
        "chinRaise" : 0,
        "lipPucker" : 1,
        "lipPress" : 0,
        "lipSuck" : 0,
        "mouthOpen" : 0,
        "smirk" : 0,
        "eyeClosure" : 0,
        "attention" : 98
    },
    "emotions" : { 
        "joy" : 0,
        "fear" : 0,
        "disgust" : 0,
        "sadness" : 0,
        "anger" : 0,
        "surprise" : 0,
        "contempt" : 0,
        "valence" : 0,
        "engagement" : 0
    }
}]

var printJson = () => console.log(JSON.stringify(sample_data))
setTimeout(printJson, 1000)
