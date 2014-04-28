light_jar
=========
## What is Light Jar?
Light Jar is arduino powered mood lamp.

## Demo 

[![ScreenShot](https://raw.githubusercontent.com/happyhj/light_jar/master/resources/light_jar_youtube_video.png)](http://www.youtube.com/watch?v=lULeXELl_bw&list=LLJAZMv_EH_lL9oyhOQU4YUA&feature=share)

## Feature
- WiFi connectivity
- Controlled by webpage

## Component
### Lamp ( arduino with WiFi-shield + 26 LED pixels )
Arduino work as web server. Set lamp status according to HTTP request path received from local-network.
### WiFi Access Point
Providing local network area. Both Lamp and client machine(such as samrt phone) should connect to same AP.
### Web Server for service webapp
Provoding Webapp(HTML,CSS) to client machine.

## Lamp API
- /api/turn_on_light 
- /api/turn_off_light		
- /api/blink_light		
- /api/blink_light/3 		
- /api/blink_light/3/change_color/100/100/100	 
- /api/dim_light/100		
- /api/change_color/100/100/200		
- /api/change_to_random_color		
- /api/turn_on_rainbow/sync		
- /api/turn_on_rainbow/cycle		
- /api/turn_on_breathe	
- /api/turn_off_show		


