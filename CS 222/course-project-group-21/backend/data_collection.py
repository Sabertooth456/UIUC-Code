## Used data provided by meteostat https://dev.meteostat.net/.
from tkinter.filedialog import asksaveasfile
import numpy as np
from datetime import datetime
from meteostat import Point,Hourly, Daily, Monthly, Stations
import matplotlib.pyplot as plt
## inputs:
## time_period: list form of length 6, each half represent year, month, day in int form.
## loc: lat, lon and alt(optional). Only works when there's a weather statino nearby.
## features:
    #hourly:
    # temp	The air temperature in °C	Float64
    # dwpt	The dew point in °C	Float64
    # rhum	The relative humidity in percent (%)	Float64
    # prcp	The one hour precipitation total in mm	Float64
    # snow	The snow depth in mm	Float64
    # wdir	The average wind direction in degrees (°)	Float64
    # wspd	The average wind speed in km/h	Float64
    # wpgt	The peak wind gust in km/h	Float64
    # pres	The average sea-level air pressure in hPa	Float64
    # tsun	The one hour sunshine total in minutes (m)	Float64
    #daily:
    # tavg	The average air temperature in °C	Float64
    # tmin	The minimum air temperature in °C	Float64
    # tmax	The maximum air temperature in °C	Float64
    # prcp	The daily precipitation total in mm	Float64
    # snow	The snow depth in mm	Float64
    # wdir	The average wind direction in degrees (°)	Float64
    # wspd	The average wind speed in km/h	Float64
    # wpgt	The peak wind gust in km/h	Float64
    # pres	The average sea-level air pressure in hPa	Float64
    # tsun	The daily sunshine total in minutes (m)	Float64
    #monthly:
    # tavg	The average air temperature in °C	Float64
    # tmin	The minimum air temperature in °C	Float64
    # tmax	The maximum air temperature in °C	Float64
    # prcp	The monthly precipitation total in mm	Float64
    # wspd	The average wind speed in km/h	Float64
    # pres	The average sea-level air pressure in hPa	Float64
    # tsun	The monthly sunshine total in minutes (m)	Float64
## freq: "hourly", "daily" or "monthly".
## outputs:
## data with cols extracted with features specified and rows as freq
def data_load(time_period, loc, features, freq):
    # input validity check
    assert(len(time_period)==6)
    assert(len(loc)>=2)
    
    # Set time period
    start = datetime(time_period[0], time_period[1], time_period[2])
    end = datetime(time_period[3], time_period[4], time_period[5])
    
    # Create Point for particular location
    if (len(loc) == 3):
        location = Point(loc[0], loc[1], loc[2])
    else:
        location = Point(loc[0], loc[1])
        
    # Get data freq
    match freq:
        case 'hourly':
            data = Hourly(location, start, end)
        case 'daily':
            data = Daily(location, start, end)
        case 'monthly':
            data = Monthly(location, start, end)
        case other:
            raise TypeError("invalid freq")
    
    data = data.fetch()
    selected = data[features]
    return selected


## visualize data input
def data_visualize(data):
    data.plot()
    plt.show()
