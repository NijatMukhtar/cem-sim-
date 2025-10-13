// DEMO verisidir!!
const DEMO = [
  {"t":0.00,"angle":0,"speed_rpm":300,"active":[0,1]},
  {"t":0.04,"angle":24,"speed_rpm":300,"active":[0,1]},
  {"t":0.08,"angle":48,"speed_rpm":300,"active":[0,1]},
  {"t":0.12,"angle":72,"speed_rpm":300,"active":[1,2]},
  {"t":0.16,"angle":96,"speed_rpm":300,"active":[1,2]},
  {"t":0.20,"angle":120,"speed_rpm":300,"active":[1,2]},
  {"t":0.24,"angle":144,"speed_rpm":300,"active":[2,3]},
  {"t":0.28,"angle":168,"speed_rpm":300,"active":[2,3]},
  {"t":0.32,"angle":192,"speed_rpm":300,"active":[3,4]},
  {"t":0.36,"angle":216,"speed_rpm":300,"active":[3,4]},
  {"t":0.40,"angle":240,"speed_rpm":300,"active":[4,5]},
  {"t":0.44,"angle":264,"speed_rpm":300,"active":[4,5]},
  {"t":0.48,"angle":288,"speed_rpm":300,"active":[5,0]},
  {"t":0.52,"angle":312,"speed_rpm":300,"active":[5,0]},
  {"t":0.56,"angle":336,"speed_rpm":300,"active":[5,0]}
];

// halka için gerekli işlemler
const N = 6;
const ring = document.getElementById('ring');
const angleEl = document.getElementById('angle');
const nodes = [];
for (let i=0;i<N;i++){
  const d = document.createElement('div');
  d.className = 'em off';
  d.style.setProperty('--rot', `${(360/N)*i}deg`);
  ring.appendChild(d);
  nodes.push(d);
}

function render(st){
  angleEl.textContent = `${st.angle.toFixed(1)}°`;
  nodes.forEach((n,i)=>{
    const on = st.active.includes(i);
    n.className = `em ${on ? 'on' : 'off'}`;
  });
}

async function play(arr){
  for (const st of arr){
    render(st);
    await new Promise(r=>setTimeout(r, 50));
  }
}

document.getElementById('play').onclick = ()=> play(DEMO);
