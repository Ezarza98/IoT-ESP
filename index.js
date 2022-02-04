'use strict';

/*
Estas dos lineas nos van a permitir la conexion con AWS IoT
La segunda linea es el objeto creado de manera que se asocia al endpoint del objeto  en AWS IoT
*/

var AWS = require('aws-sdk'); 

var iotdata = new AWS.IotData({ endpoint: '*******' }); //Put here the endpoint of your skill

/*
Esta linea de debajo nos va a permitir la actualizacion de la base de datos
*/
var docClient = new AWS.DynamoDB.DocumentClient({region:"*******"}); //Put here the region where is your amazon acount
var table ="*******"; //Put here the name of the AmazonDB that you will use


//--------------------------- Ahora las funciones que van a permitir responder------------------------------//

function RespuestaHablada(title, output, repromptText, shouldEndSession) {
    /*
    Esta funcion crea el mensaje que va a recibir e interpretar alexa
    title:
    output:
    repromtText:
    shouldEndSession:
    */
    return {
        outputSpeech: {
            type: 'PlainText',
            text: output,
        },
        card: {
            type: 'Simple',
            title: `SessionSpeechlet - ${title}`,
            content: `SessionSpeechlet - ${output}`,
        },
        reprompt: {
            outputSpeech: {
                type: 'PlainText',
                text: repromptText,
            },
        },
        shouldEndSession,
    };
}

function CreacionRespuesta(sessionAttributes, speechletResponse) {
    /*
    Esta funcion crea el mensaje Json que es el tipo de mensaje que puede recibir Alexa. 
    Si hago todo en una sola funcion (RespuestaHablada y CreacionRespuesta) da error. hay que hacerlo en dos
    sessionAttributes:
    speechletResponse: Esto es el mensaje que lee Alexa y que puede "pronunciar" al usuario
    */
    return {
        version: '1.0',
        sessionAttributes,
        response: speechletResponse,
    };
}


//--------------------------------Estas funciones van a controlar las invocaciones de Alexa a la funcion-----------------------//


//Funcion LAUNCH REQUEST
function RespuestaLaunchRequest(callback) {
    // If we wanted to initialize the session to have some attributes we could add those here.
    //Funcion para generar la respuesta del LaunchRequest
    const sessionAttributes = {};
    const cardTitle = 'Welcome';
    const speechOutput = 'Bienvenido. ' + 'Dígame que puedo hacer.';
    // Si el usuario no dice nada en el tiempo en el que alexa mantiene la sesion abierta
    // o dice algo que no esta contemplado se va a usar el repromptText
    const repromptText = 'Si no dice nada voy a cerrar sesion, ' + 'prueba a decirme que apague todas las luces.';
    const shouldEndSession = false; 
    var params = { //Datos del envio al servidor MQTT
                    topic: "MyTopic/iot/casa", //Topic en el que vamos a publicar
                    payload: JSON.stringify("PRUEBA DE MENSAJE"), //Dentro de este mensaje donde vamos a poner lo que se publique
                    qos: 0
                };

    iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                    if (err) {
                        console.log("ERROR => " + JSON.stringify(err));
                        
                    }
                    else {
                        console.log("Success");
                         }
                }).promise();
                
    callback(sessionAttributes,
        RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));
}


//Funcion HELP REQUEST
function RespuestaHelpRequest(callback) {
    // If we wanted to initialize the session to have some attributes we could add those here.
    //Funcion para generar la respuesta del LaunchRequest
    const sessionAttributes = {};
    const cardTitle = 'AYUDA';
    const speechOutput = 'Esta skill sirve para control del hogar' + 'Es un TFG de Eduardo Zarza'
                        + 'Puedes pedirme que apague la luz de una habitación concreta'
                        +'También puedes pedirme que apague o que encienda todas las luces';
    
    // Si el usuario no dice nada en el tiempo en el que alexa mantiene la sesion abierta
    // o dice algo que no esta contemplado se va a usar el repromptText
    
    const repromptText = 'Veo que no necesita mis servicios, ' + 'procedo a cerrar la skill.';
    
    const shouldEndSession = false;
    
    callback(sessionAttributes,
        RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));
}



//Funcion STOP REQUEST
function RespuestaCierreSkill(callback) {
    //Generar respuesta cuando mandamos cerrar la skill
    const cardTitle = 'Session Ended';
    const speechOutput = 'Gracias por probar la Skill de mi TFG. Espero que me vuelva a usar pronto.';
    //Mandamos un TRUE en shouldEndSession para que deje de estar a la espera de que hablemos
    const shouldEndSession = true;

    callback({}, RespuestaHablada(cardTitle, speechOutput, null, shouldEndSession));
}

//Funcion para encender todas las luces
function EncendidoGeneral(intent, session, callback){
    const cardTitle = intent.name;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';
    
    speechOutput = `Acabas de encender las luces de cocina, salon, patio, baño y habitación. `;
    repromptText = "¿Deseas que haga otra acción?";
    
    var params = { //Datos del envio al servidor MQTT
            topic: "MyTopic/iot/casa/luz/encender", //Topic en el que vamos a publicar
            payload: JSON.stringify("1"), //Dentro de este mensaje donde vamos a poner lo que se publique
            qos: 0
            };

    iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                        
                }
                else {
                    console.log("Success");
                        }
    }).promise();
    callback(sessionAttributes,
         RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));

}

//Funcion para apagar todas las luces
function ApagadoGeneral(intent, session, callback){
    const cardTitle = intent.name;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';
    
    speechOutput = `Acabas de apagar las luces de cocina, salon, patio, baño y habitación. `;
    repromptText = "¿Deseas que haga otra acción?";
    
    var params = { //Datos del envio al servidor MQTT
            topic: "MyTopic/iot/casa/luz/apagar", //Topic en el que vamos a publicar
            payload: JSON.stringify("1"), //Dentro de este mensaje donde vamos a poner lo que se publique
            qos: 0
            };

    iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                        
                }
                else {
                    console.log("Success");
                        }
    }).promise();
    callback(sessionAttributes,
         RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));

}

//Funcion para el REQUEST de encender las luces
function EnciendeLuz(intent, session, callback) {  //Esta envia un 1 para encender la luz que indiquemos
    console.log(intent);
    const cardTitle = intent.name;
    const habitacion = intent.slots.sitioEN;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';

    if (habitacion) {
        let lugar = habitacion.value; //Lugar es el sitio donde vamos a actuar
        speechOutput = `Acabas de encender la luz de ${lugar}. `;
        repromptText = "¿Deseas que haga otra acción?";
        var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/"+lugar+"/luz/encender", //Topic en el que vamos a publicar
                payload: JSON.stringify("1"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
                };
        iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                    if (err) {
                        console.log("ERROR => " + JSON.stringify(err));
                    }
                    else {
                        console.log("Success");
                         }
                }).promise();
    } else {
        speechOutput = "No he entendido lo que me has dicho. Repite por favor.";
        repromptText = "Prueba a decir , enciende luz de la cocina";
    }
    callback(sessionAttributes,
         RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));
}


//Funcion de APAGAR LUCES

function ApagaLuz(intent, session, callback) {  //Esta envia un 1 para encender la luz que indiquemos

    const cardTitle = intent.name;
    const habitacion = intent.slots.sitioAP;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';

    if (habitacion) {
        let lugarAP = habitacion.value; //Lugar es el sitio donde vamos a actuar
        
        speechOutput = `Acabas de apagar la luz de ${lugarAP}. `;
        repromptText = "¿Deseas que haga otra acción?";
        
        var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/"+lugarAP+"/luz/apagar", //Topic en el que vamos a publicar
                payload: JSON.stringify("0"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
                };

        iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                    if (err) {
                        console.log("ERROR => " + JSON.stringify(err));
                        
                    }
                    else {
                        console.log("Success");
                         }
                }).promise();
        
    } else {
        speechOutput = "No he entendido lo que me has dicho. Repite por favor.";
        repromptText = "Prueba a decir , enciende luz de la cocina";
    }

    callback(sessionAttributes,
    RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));
}


//Funcion PEDIR DATOS SENSOR
var TEMPERATURA = '';
var HUMEDAD = '';
function DatoSensor(intent, session, callback){
    const cardTitle = intent.name;
    const tipo = intent.slots.dato;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';
    let variable = tipo.value;
    if (tipo){
        if (variable==='humedad') {
            var parametros = {
                TableName:table,
                Key:{
                    "SensorID":"Humedad"
                },
            };    
            docClient.get(parametros, function(err, data) {
                if (err) {
                    console.error("No se ha podido leer el dato. Error JSON:", JSON.stringify(err, null, 2));
                } else {
                    console.log("Lectura correcta:", JSON.stringify(data,null,2));
                    HUMEDAD=data.Item.Valor;
                    console.log(HUMEDAD);
                }
            });

           speechOutput = `Actualmente hay un ` + HUMEDAD + ` porciento de humedad. `;
           repromptText = "¿Desea algo mas?";
        }else if(variable==='temperatura') {
            var parametros = {
                TableName:table,
                Key:{
                    "SensorID":"Temperatura"
                },
            };
            docClient.get(parametros, function(err, data) {
                if (err) {
                    console.error("No se ha podido leer el dato. Error JSON:", JSON.stringify(err, null, 2));
                } else {
                    console.log("Lectura correcta:", JSON.stringify(data, null, 2));
                    TEMPERATURA=data.Item.Valor;
                    console.log(TEMPERATURA);
                }
            });
            speechOutput = `Actualmente hay` + TEMPERATURA + `grados centígrados`;
            repromptText = "¿Desea algo mas?";
         }
     }else{            
            speechOutput = "No he entendido lo que me has dicho. Repite por favor.";
            repromptText = "Prueba a decir , dime la temperatura";
         }
    callback(sessionAttributes,
    RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));        
}

//Funcion para actualizar la tabla con valores de los sensores. Esto envia peticion al ESP para actualizar los datos
function PideDatosSensores(intent, session, callback){
    const cardTitle = intent.name;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';
    
    speechOutput = `Acabas de actualizar los datos de los sensores. Prueba a pedirme los datos`;
    repromptText = "¿Deseas que haga otra acción?";
    
    var params = { //Datos del envio al servidor MQTT
            topic: "MyTopic/iot/casa/sensor", //Topic en el que vamos a publicar
            payload: JSON.stringify("1"), //Dentro de este mensaje donde vamos a poner lo que se publique
            qos: 0
            };

    iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                        
                }
                else {
                    console.log("Success");
                        }
    }).promise();
    callback(sessionAttributes,
    RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));        
    
}


//Funcion para el control de persianas
function ControlPersianas(intent, session, callback){
    const cardTitle = intent.name;
    const comando = intent.slots.comando;
    let repromptText = '';
    let sessionAttributes = {};
    const shouldEndSession = false;
    let speechOutput = '';
    let variable = comando.value;
    if (comando){
        if (variable==='desactivar' || variable==='desactiva' || variable==='cero' ||variable==='0') {
            speechOutput = `Acabas de desactivar el control automático de las persianas`;
            repromptText = "¿Deseas que haga otra acción?";
    
            var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/persianas", //Topic en el que vamos a publicar
                payload: JSON.stringify("0"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
            };
            iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                }
                else {
                    console.log("Success");
                        }
            }).promise();
        }else if(variable==='activar' || variable==='activa'){
            speechOutput = `Acabas de activar el control automático de las persianas`;
            repromptText = "¿Deseas que haga otra acción?";
    
            var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/persianas", //Topic en el que vamos a publicar
                payload: JSON.stringify("1"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
            };
            iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                }
                else {
                    console.log("Success");
                        }
            }).promise();            
        }else if(variable==='uno' || variable==='mañana' || variable==='alto' || variable === '1'){
            speechOutput = `Acabas de activar el control automático de las persianas en modo de mañana`;
            repromptText = "¿Deseas que haga otra acción?";
    
            var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/persianas", //Topic en el que vamos a publicar
                payload: JSON.stringify("2"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
            };
            iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                }
                else {
                    console.log("Success");
                        }
            }).promise();            
        }else if(variable==='tarde' || variable==='trabajo' || variable==='dos' || variable==='medio' || variable==='2'){
            speechOutput = `Acabas de activar el control automático de las persianas en modo trabajo`;
            repromptText = "¿Deseas que haga otra acción?";
    
            var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/persianas", //Topic en el que vamos a publicar
                payload: JSON.stringify("3"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
            };
            iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                }
                else {
                    console.log("Success");
                        }
            }).promise();
        }else if(variable==='noche' || variable==='descanso' || variable==='bajo' || variable==='tres' || variable==='3' ){
            speechOutput = `Acabas de activar el control automático de las persianas en modo descanso`;
            repromptText = "¿Deseas que haga otra acción?";
    
            var params = { //Datos del envio al servidor MQTT
                topic: "MyTopic/iot/casa/persianas", //Topic en el que vamos a publicar
                payload: JSON.stringify("4"), //Dentro de este mensaje donde vamos a poner lo que se publique
                qos: 0
            };
            iotdata.publish(params, function(err, data) { //Funcion para enviar los datos a MQTT
                if (err) {
                    console.log("ERROR => " + JSON.stringify(err));
                }
                else {
                    console.log("Success");
                        }
            }).promise();
        }else{
            speechOutput = "No he entendido lo que me has dicho. Repite por favor.";
            repromptText = "Prueba a decir , activa control de persianas";            
        }
    }else{
            speechOutput = "No he entendido lo que me has dicho. Repite por favor.";
            repromptText = "Prueba a decir , activa control de persianas";
         }
    callback(sessionAttributes,
    RespuestaHablada(cardTitle, speechOutput, repromptText, shouldEndSession));
}
//----------------------FUNCIONES PARA CUANDO EL AWS IOT INVOQUE A LAMBDA-------------------//

//Funcion actualizar tabla humedad
function actualizahumedad(event){
    HUMEDAD = event.message;
    var SensorID="Humedad";
    var parametros = {
        TableName:table,
        Key:{
          "SensorID":SensorID
        },
        UpdateExpression: "set Valor = :r",
        ExpressionAttributeValues:{
            ":r": HUMEDAD,
        },
        ReturnValues:"UPDATED_NEW"
    };   
    
    docClient.update(parametros, function(err, data) {
        if (err) {
            console.error("Error en la actualizacion. Error JSON:", JSON.stringify(err, null, 2));
        } else {
            console.log("Actualizacion correcta:", JSON.stringify(data, null, 2));
        }
    });
       console.log(parametros)
       
}

//Funcion actualizar tabla temperatura
function actualizatemperatura(event){
    TEMPERATURA = event.message;
    var SensorID="Temperatura";
    var parametros = {
        TableName:table,
        Key:{
          "SensorID":SensorID
        },
        UpdateExpression: "set Valor = :r",
        ExpressionAttributeValues:{
            ":r": TEMPERATURA,
        },
        ReturnValues:"UPDATED_NEW"
    };   
    
    docClient.update(parametros, function(err, data) {
        if (err) {
            console.error("Error en la actualizacion. Error JSON:", JSON.stringify(err, null, 2));
        } else {
            console.log("Actualizacion correcta:", JSON.stringify(data, null, 2));
        }
    });
       console.log(parametros)
       
}

// --------------- EVENTOS DE ALEXA -------------------------------------//

/**
 * Para cada inicio de sesion. Obtiene los datos de los eventos de 
 */
function onSessionStarted(sessionStartedRequest, session) {
    console.log(`onSessionStarted requestId=${sessionStartedRequest.requestId}, sessionId=${session.sessionId}`);
}

/**
 * Cuando se ralice la invocacion de la skill.
 */
function onLaunch(launchRequest, session, callback) {
    console.log(`onLaunch requestId=${launchRequest.requestId}, sessionId=${session.sessionId}`);

    // ejecuta la funcion de launch.
    RespuestaLaunchRequest(callback); //callback tiene como datos 
}

/**
 * Cuando se invoca una intent concreta se ejecuta esta funcion.
 */
function onIntent(intentRequest, session, callback) {
    console.log(`onIntent requestId=${intentRequest.requestId}, sessionId=${session.sessionId}`);

    const intent = intentRequest.intent;
    const intentName = intentRequest.intent.name;

    // Dispatch to your skill's intent handlers
    if (intentName === 'EnciendeLuzIntent') {
        EnciendeLuz(intent, session, callback);
        
    }else if (intentName === 'ApagaLuzIntent') {
        ApagaLuz(intent, session, callback);
        
    }else if (intentName === 'EncendidoGeneralIntent') {
        EncendidoGeneral(intent, session, callback);
        
    }else if (intentName === 'ApagadoGeneralIntent') {
        ApagadoGeneral(intent, session, callback);
        
    }else if (intentName === 'ControlPersianaIntent') {
        ControlPersianas(intent, session, callback);
        
    }else if (intentName === 'ActualizaSensoresIntent') {
        PideDatosSensores(intent, session, callback);
        
    }else if (intentName === 'InformacionSensorIntent') {
        DatoSensor(intent, session, callback);
        
    }else if (intentName === 'AMAZON.HelpIntent') {
        RespuestaHelpRequest(callback);
        
    } else if (intentName === 'AMAZON.StopIntent' || intentName === 'AMAZON.CancelIntent') {
        RespuestaCierreSkill(callback);
        
    }  else {
        throw new Error('Intent no contemplada');
    }
}

/**
 * Called when the user ends the session.
 * Is not called when the skill returns shouldEndSession=true.
 */
function onSessionEnded(sessionEndedRequest, session) {
    console.log(`onSessionEnded requestId=${sessionEndedRequest.requestId}, sessionId=${session.sessionId}`);
    // Add cleanup logic here
}


// --------------- Main handler -----------------------

// Route the incoming request based on type (LaunchRequest, IntentRequest,
// etc.) The JSON body of the request is provided in the event parameter.

//Cada vez que uno de los desencadenadores puestos arriba envia una peticion par ainvoacr la funcion, se ejecutan las lineas de debajo
//De manera que se obtienen tres atributos, event(datos de lo que invoca), context(), y callback()
exports.handler = (event, context, callback) => {

    try {
//----------------PRIMERO LOS CASOS DE INVOCACION POR PARTE DE ALEXA-----------------------//
        if (event.session.new) {
            onSessionStarted({ requestId: event.request.requestId }, event.session);
        }

        if (event.request.type === 'LaunchRequest') {
            onLaunch(event.request,
                event.session,
                (sessionAttributes, speechletResponse) => {
                    callback(null, CreacionRespuesta(sessionAttributes, speechletResponse));
                });
        } else if (event.request.type === 'IntentRequest') {
            onIntent(event.request,
                event.session,
                (sessionAttributes, speechletResponse) => {
                    callback(null, CreacionRespuesta(sessionAttributes, speechletResponse));
                });
        } else if (event.request.type === 'SessionEndedRequest') {
            onSessionEnded(event.request, event.session);
            callback();
        }
//-----------------AHORA LOS CASOS DE INVOCACION POR LA REGLA DE AWS IoT-----------------------//        
        else if(event.topic==='T'){
            actualizatemperatura(event);
        }else if(event.topic==='H'){
            actualizahumedad(event);
        }
    }catch (err) {
        callback(err);
    }
};