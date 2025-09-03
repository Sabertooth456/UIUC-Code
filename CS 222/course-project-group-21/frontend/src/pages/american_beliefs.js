import React from "react";

import survey from "./PewSurvey.png"

const Beliefs = () => {
  return (
    <div className="d-flex justify-content-center">
      <div>
        <h1>
          American Beliefs About Climate Change
        </h1>
          <p>
            Most Americans say that the federal government isn't doing enough to protect the environment, regardless of benefits to humans or the nature.
            They believe that the United States should focus on developing alternative sources of fuel instead of expanding the use of fossil fuels.
          </p>
          <p>
            However, Democrats and Republicans are split over the government's handling of climate change.
            90% of Democrats believe the government is doing too little, while just under 40% of Republicans believe the same.
            Similarly, Moderates/Liberals believe the government isn't doing enough, while few Conservatives say the same.
            Age also has a negative correlation to concern for the climate.
          </p>
          <p>
            While the majority of Americans believe the government isn't doing enough to protect the environment, climate change is still a divisive, ideological issue.
          </p>
      </div>
      <div>
        <img src={survey} alt="Survey done by Pew Research Center"/>
      </div>
      <h4>Reference:</h4>
      <p>https://www.pewresearch.org/science/2019/11/25/u-s-public-views-on-climate-and-energy/</p>
    </div>
  );
};
  
export default Beliefs;