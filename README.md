# 1. 기능
한 이미지 내에서 template을 설정한 후, 그 template과 일치하는 pattern을 **여러 개를** 찾을 수 있다.<br>
그리고 matching 찾을 때의 **threshold** 및 **여러가지 방법**들을 실시간 설정가능하도록 gui도 구현<br><br>

### 참고자료 
 - https://docs.opencv.org/3.4/de/da9/tutorial_template_matching.html <br>
 - https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_imgproc/py_template_matching/py_template_matching.html

<br><br>
# 2. 전체적인 모습
![프로그램 실행 모습](https://user-images.githubusercontent.com/43025974/50533823-ca064b00-0b75-11e9-9dd6-39f0c466bd72.png)
<br><br>

# 3. 사용법
 1. 입력 이미지가 들어가면 ROI란 창이 뜰 것이다.
 2. 거기서 마우스 드래그를 통해 원하는 template을 설정 가능
 3. matching에 사용된 방식에 따른 결과 이미지가 Controller 창에 나타난다.
 4. 이 결과이미지의 최대/최소 값을 찾으며, 그 값이 threshold보다 높을 때 까지 match 지점을 찾는다.(최대 20개)
 5. 최종 결과 이미지에 template matching된 영역이 표시된다.
