import React from "react";

import chart from "./Arctic Sea Ice Chart.png"
import visual from "./Change In Arctic Ice.png"

const Ice = () => {
  return (
    <div className="d-flex justify-content-center">
      <div>
        <h1>
          Projections of Arctic Sea Ice
        </h1> 
        <p>
          Since 1980, measurements of total ice covering the Arctic have projected a downward trend, starting from around 7 million square kilometers in 1980 to 5 million in 2022.
        </p>
      </div>
      <div>
        <img src={chart} alt="Average Monthly Sea Ice"/>
      </div>
        <p>
          However, the ice caps are necessary for balancing the Earth's energy, keeping the polar regions cool by reflecting sunlight back into space and forming an insulating barrier between the cold air above it and the warm oceans below it.
          It regulates temperature, ocean circulation, and animal habits, but its efficacy has dimished due to rising temperatures.
        </p>
      <div>
        <img src={visual} alt="Change In Arctic Sea Ice"/>
      </div>
      <h4>Reference:</h4>
      <p>https://www.arcus.org/sipn/sea-ice-outlook/2022/post-season</p>
    </div>
  );
};
  
export default Ice;