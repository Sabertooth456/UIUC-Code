import React from 'react';
import './App.css';
import Navbar from './components/Navbar';
import { BrowserRouter as Router, Routes, Route}
	from 'react-router-dom';
import Home from './pages';
import Beliefs from './pages/american_beliefs';
import Ice from './pages/arctic_ice';
import Fight from './pages/fight_cc';
import Temperatures from './pages/global_temperatures';
import Variables from './pages/variables_cc';

function App() {

return (
	<Router>
	<Navbar />
	<Routes>
		<Route exact path='/home' element={<Home />} />
		<Route path='/american_beliefs' element={<Beliefs/>} />
		<Route path='/arctic_ice' element={<Ice/>} />
		<Route path='/fight_cc' element={<Fight/>} />
		<Route path='/global_temperatures' element={<Temperatures/>} />
    	<Route path='/variables_cc' element={<Variables/>} />
	</Routes>
	</Router>
);
}

export default App;
