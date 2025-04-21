# Process Identification

When the user clicks on the interface of the guiding robot, they can match the user with the corresponding patient data in the database through 1. facial recognition and 2. inputting information such as name, date of birth, gender, etc.

![body](image/facial recognition.jpg)

If a match is found with the patient data in the database, the corresponding department for consultation will be displayed and the navigation function will guide the user to enter the corresponding department automatically. If no match is found, the user clicks on the navigation button. According to the map displayed on the interface, they select the corresponding location and click "Start Navigation" to start the guiding robot. 

![body](image/map.jpg)

During the navigation process, the guiding robot uses ultrasonic waves to detect whether there are obstacles ahead and sets the safe distance at 20 cm. If the distance between the guiding robot and the obstacle is less than the safe distance, the guiding robot stops moving and waits until the distance between the guiding robot and the obstacle is greater than the safe distance before continuing to move and guiding. During the guiding process, the guiding robot plays pre-recorded audio: "start, hold, stop".
