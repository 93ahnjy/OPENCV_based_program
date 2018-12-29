# 1. 기능
한 이미지 내에서 template을 설정한 후, 그 template과 일치하는 pattern을 **여러 개를** 찾을 수 있다.<br>
그리고 matching 찾을 때의 **threshold** 및 **여러가지 방법**들을 실시간 설정가능하도록 gui도 구현<br>
(이미지 경로는 알아서 조정)<br>

 ### ※ 참고자료 
 - https://docs.opencv.org/3.4/de/da9/tutorial_template_matching.html <br>
 - https://docs.opencv.org/3.0-beta/doc/py_tutorials/py_imgproc/py_template_matching/py_template_matching.html

 ### ※ 사용함수
```c
matchTemplate(img_gray, templ, result, method) // 주요 함수;
cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, general_mask)  // 해당 이미지 내에 최대/최소값 및 그 위치들 기록;
```


<br><br>
# 2. 전체적인 모습
![프로그램 실행 모습](https://user-images.githubusercontent.com/43025974/50533823-ca064b00-0b75-11e9-9dd6-39f0c466bd72.png)
<br><br>

# 3. 사용법
 **1)** 입력 이미지가 들어가면 ROI란 창이 뜰 것이다.<br>
 **2)** 거기서 마우스 드래그를 통해 원하는 template을 설정 가능<br>
 **3)** matching에 사용된 방식에 따른 결과 이미지가 Controller 창에 나타난다.<br>
 **4)** 이 결과이미지의 최대/최소 값을 찾으며, 그 값이 threshold보다 높을 때 까지 match 지점을 찾는다.(최대 20개)<br>
 **5)** 최종 결과 이미지에 template matching된 영역이 표시된다.<br>
<br><br>

# 4. 몇 가지 기록
 ### 1) ROI 잡기
 
```c
int mouse_x = cvui::mouse().x;
int mouse_y = cvui::mouse().y;


mouse_x = mouse_x < 0 ? 0 : mouse_x;    // 지금 내 마우스 위치가 window 북서쪽으로 벗어났는가?
mouse_y = mouse_y < 0 ? 0 : mouse_y;

int width = mouse_x - anchor.x;
int height = mouse_y - anchor.y;


ROI.x = width < 0 ? anchor.x + width : anchor.x;    // 지금 내 마우스 위치가 ROI의 북서쪽에 위치하는가?
ROI.y = height < 0 ? anchor.y + height : anchor.y;

ROI.width = std::abs(width);
ROI.height = std::abs(height);

                                                     // 지금 내 마우스 위치가 window 남동쪽으로 벗어났는가?

ROI.width = ROI.x + ROI.width > img_cpy.cols ? ROI.width + img_cpy.cols - (ROI.x + ROI.width) : ROI.width;
ROI.height = ROI.y + ROI.height > img_cpy.rows ? ROI.height + img_cpy.rows - (ROI.y + ROI.height) : ROI.height;
```
<br>

 ### 2) Matching 방식에 따른 macth 지점 구하기 및 thershold 설정

```c
cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, general_mask);

if (method == TM_SQDIFF || method == TM_SQDIFF_NORMED)
{
	matchLoc = minLoc;
	if (minVal > 1-threshold || match_num > 20)
		break;
}
else
{
	matchLoc = maxLoc;
	if (maxVal < threshold || match_num > 20)
		break;
}
```
<br>

### 3) Matching 주변을 또 찾지 않게 match 지점 주변 masking 해버리기
```c
Mat template_mask = Mat::zeros(template_h, template_w, CV_8UC1);
template_mask.copyTo(general_mask(cv::Rect(x, y, template_w, template_h)));
```
<br>

# 5. 결론
즉, template으로 스캐닝해서 나온 흑백 이미지에서 min, max 값을 계속 찾고, 찾을 때마다 그 주변을 masking한 뒤에 다음 min, max를 찾는 식으로 해서
threshold를 더이상 못넘을 때까지 전부 찾는 방식이다.  
