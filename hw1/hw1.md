# HW1
Author: [b07902125](mailto:b07902125@ntu.edu.tw) 鄭昊昕 \
Paper Name：The Design Philosophy of the DARPA Internet Protocols
## 心得
本篇論文主要在解釋當時網路架構設計以及部份網路通訊協定設計的緣由及目的。比如為了達到Multiplexing的需求，之所以選擇將網路架構設計成packet switching，而非circuit switching的其中一個原因:例如遠端登入這類服務可以很自然的在packet switching的架構上提供，而且可以整合數個網路(e.g家裡區域網路/地方ISP的網路/學校網路)。 \
另外，在Second Level Goal的部份中有提到，關於"effective"的網路架構，在不同的需求/環境下會有不同的設計理念。作者提供了一個checklist，並強調這個checklist中，每個項目的重要性並非固定，改變順序就會產生不同的網路架構。例如戰時的軍用網路和平時的商用網路著重的點就有所不同。 \
此外，原本設計TCP時是希望它能夠支援任何種類的服務，但是由於不同的服務對於延遲、頻寬、錯誤率有不同的要求，故後來做出了一些修正。例如，對於VoIP服務來說，它對於延遲的要求較高，但對於錯誤率的要求並不像是傳遞文字訊息，允許遺漏部份內容。而以HTTP這類建立在TCP上的應用來說，由於不需要即時反應，故對低延遲的需求不高，但是注重錯誤率，Server盡可能的呈現完整無誤的內容給Client。因此後來將TCP分成TCP/IP兩部份，讓TCP專注於提供特定種類(如:需正確率)的服務，而IP則處理datagram的部份，也可以在網路連線不穩定的環境中使用。\
在本文當中有提到的X.25 network，在1980-90年代曾普遍被使用，為一個依照OSI Model建立的網路協定。但在現今的internet發展下逐漸被取代，使用Frame Relay的技術來取代，並在ADSL,ISDN等新技術的推出後漸漸消失在市場上。\
