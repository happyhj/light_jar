light_jar
=========
## What is Light Jar
Light Jar is arduino powered mood lamp.

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
조명 켜기
- /api/turn_off_light		
조명 끄기
- /api/blink_light		
한번 깜박
- /api/blink_light/3		
기존 색으로 N번 깜박
- /api/blink_light/3/change_color/100/100/100	
설정한 색으로 N번 깜박
- /api/dim_light/100		
밝기를 주어진 값으로 조절
- /api/change_color/100/100/200		
조명의 색상을 주어진 색으로 변경
- /api/change_to_random_color		
조명의 색상을 렘덤으로 변경
- /api/turn_on_rainbow/sync		
무지개 쇼(동일 색) 모드를 켜기
-/api/turn_on_rainbow/cycle		
무지개 쇼(전구마다 다른 색 모드를 켜기
- /api/turn_on_breathe	
조명 색으로 숨쉬기 쇼 켜기
- /api/turn_off_show		
쇼 모드를 끄고 조명 모드로



