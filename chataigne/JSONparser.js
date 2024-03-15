// Set module params
local.parameters.protocol.set("Lines");
local.parameters.autoAdd.set(false);

// Set script params
script.enableLog.set(true);

// Add a test trigger
var testTrigger = local.values.addTrigger("Test Trigger", "Description of my trigger");

// Add a container for sensors
var sensorsContainer = local.values.addContainer("Sensors");

// Initialize an empty array to store the sensors objects
var sensors = [];

// Add sensors to the sensors array
for(var i = 0; i < 8; i++) {

	// Add a container for each sensor
	sensors.push(sensorsContainer.addContainer("Sensor " + (i+1)));

	for(var j = 0; j < 4; j++) {
		// Add parameters for each sensor inside this container
		sensors[i].addIntParameter("Distance" + (j+1) + "Value", "Distance" + (j+1) + "Value", 0);
		sensors[i].addIntParameter("Distance" + (j+1) + "Status", "Distance" + (j+1) + "Status", 0);
		sensors[i].addBoolParameter("Distance" + (j+1) + "IsPresent", "Distance" + (j+1) + "IsPresent", 0);
	}
}

// Method called by the test trigger
function testFunction() {
	script.log("Test function");
}

// Method called when a module value changes
function moduleValueChanged(value) {
	if(value.is(testTrigger)) {
		testFunction();	
	}
}

// Method called when data is received on the serial port
function dataReceived(data) {

	// Parse the received data into a JSON object
	var sensorDatas = JSON.parse(data);

	// Get the sensors array from the received JSON object
	var receivedSensors = sensorDatas.params.sensors;

	// Update the sensors values
	for(var i = 0; i < receivedSensors.length; i++) {
		var receivedSensor = receivedSensors[i];
		var receivedSensorId = receivedSensors[i].id;
		var indexOfSensorToUpdate = receivedSensorId - 1;
		var objectsFound = receivedSensor.objects.length;

		// For each object found by the physical sensor, update the values
		for(var j = 0; j < objectsFound; j++) {
			sensors[indexOfSensorToUpdate].getChild("Distance" + (j+1) + "Value").set(receivedSensor.objects[j].distance);
			sensors[indexOfSensorToUpdate].getChild("Distance" + (j+1) + "Status").set(receivedSensor.objects[j].status);
			sensors[indexOfSensorToUpdate].getChild("Distance" + (j+1) + "IsPresent").set(true);
		}

		// For each object not found by the physical sensor, reset the values
		for(var j = objectsFound; j < 4; j++) {
			sensors[indexOfSensorToUpdate].getChild("Distance" + (j+1) + "Value").set(0);
			sensors[indexOfSensorToUpdate].getChild("Distance" + (j+1) + "Status").set(255);
			sensors[indexOfSensorToUpdate].getChild("Distance" + (j+1) + "IsPresent").set(false);
		}
	}
}
