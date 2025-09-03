import React from "react";

import projection from "./Climate Change Projections.png"

const Temperatures = () => {
  return (
    <div className="d-flex justify-content-center">
      <div>
        <h1>
          Projections of Global Temperatures
        </h1>
      </div>
      <p>
        There is a large disrepancy between current policies, current pledges, and current targets.
      </p>
      <div>
        <img src={projection} alt="Climate Change Projections"/>
      </div>
      <p>
        Given the current handling of climate change, it will result in average temperatures rising between 2.6-2.9 degrees Celsius above pre-industrial levels.
        The current 2030 targets expect temperatures to rise by 2.4 degrees Celsius, while the most optimistic projections resulted in around 1.8 degrees of warming.
        Greenhouse gases must be significantly reduced in the 2020s and 2030s in order to achieve an average warming of 1.5 degrees Celsius by 2100.
      </p>
      <h4>Reference:</h4>
      <p>https://climateactiontracker.org/global/temperatures/#:~:text=Current%20policies%20presently%20in%20place,C%20above%20pre-industrial%20levels.</p>
    </div>
  );
};
  
export default Temperatures;