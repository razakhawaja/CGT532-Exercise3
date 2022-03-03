import processing.serial.*;
import http.requests.*;
Serial myPort;
String serialValue;
String apiKey = "cFs0X5unUEG2OY3_hgIn82";
String event1 = "bass_tune";
String event2 = "music_update1";
String event3 = "music_update2";
String event4 = "music_update3";

void setup() {
  myPort = new Serial(this, "/dev/cu.usbmodem1101", 9600);
}

void draw() {
  if (myPort.available() > 0) {
    serialValue = myPort.readString().trim(); // Reading the string and trimming whitespace.
    println(serialValue);
    if (serialValue.contains("Loud")) { // Fire webhook for phone notification.
      GetRequest get = new GetRequest("https://maker.ifttt.com/trigger/" + event1 + "/json/with/key/" + apiKey);
      get.send();
      
      // Fire webhook for GroupMe texts for each light sequence.
    } else if (serialValue.contains("Seq-A")) { 
      GetRequest get = new GetRequest("https://maker.ifttt.com/trigger/" + event2 + "/json/with/key/" + apiKey);
      get.send();
    } else if (serialValue.contains("Seq-B")) { 
      GetRequest get = new GetRequest("https://maker.ifttt.com/trigger/" + event3 + "/json/with/key/" + apiKey);
      get.send();
    } else if (serialValue.contains("Seq-C")) { 
      GetRequest get = new GetRequest("https://maker.ifttt.com/trigger/" + event4 + "/json/with/key/" + apiKey);
      get.send();
    }
  }
}
