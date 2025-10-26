const $ = id => document.getElementById(id);
const wrap360 = a => (a%360 + 360) % 360;
const clamp = (x,a,b)=> Math.max(a, Math.min(b, x));
const angDiff = (a,b)=> { let d = Math.abs(wrap360(a) - wrap360(b)); return d>180 ? 360-d : d; };


const arena = $("arena"), discWrap=$("discWrap"), stats=$("stats");
const inpN=$("inpN"), inpMode=$("inpMode"), inpDir=$("inpDir"),
      inpRPM=$("inpRPM"), inpStep=$("inpStep"), inpOverlap=$("inpOverlap"), inpPhase=$("inpPhase"),
      inpAngles=$("inpAngles");
const btnStart=$("btnStart"), btnStop=$("btnStop"), btnReset=$("btnReset");
const btn4x90=$("btn4x90"), btn6x60=$("btn6x60"), btn8x45=$("btn8x45"), btn12x30=$("btn12x30");


let N=4, mode=2, dir=1, rpm=300, stepMs=40, overlapDeg=20, phaseDeg=0;
let angles=[0,90,180,270];  
let t=0, ang=0, raf=null, last=0; 


function cssNum(el, name){ return parseFloat(getComputedStyle(el).getPropertyValue(name)); }
function trackRadius(){ const ring = cssNum(document.documentElement,'--ring'); const inset=cssNum(document.documentElement,'--trackInset'); return ring/2 - inset; }
function ringCenter(){ return { cx: arena.clientWidth/2, cy: arena.clientHeight/2 }; }


let logFrames = [];   
let playing = false;

document.getElementById('loadLog').addEventListener('change', async (e)=>{
  const file = e.target.files?.[0];
  if(!file) return;
  const text = await file.text();
  logFrames = text.split(/\r?\n/).map(s=>s.trim()).filter(Boolean).map(JSON.parse);
  console.log('[log] frames:', logFrames.length);
});

document.getElementById('btnPlayLog').addEventListener('click', ()=>{
  if(!logFrames.length) { alert('Önce bir JSONL dosyası seç.'); return; }
  if(raf){ cancelAnimationFrame(raf); raf=null; } 
  playing = true;
  playLogFromFrames(logFrames);
});

async function playLogFromFrames(frames){
  for (let i = 0; i < frames.length && playing; i++) {
    const f = frames[i];
    t   = Number(f.t) || 0;                 
    ang = wrap360(f.disk_angle);            
    rpm = (f.rpm != null ? f.rpm : rpm);    
    render();
    const dt = (i > 0) ? Math.max(0, (frames[i].t - frames[i-1].t) * 1000) : 16;
    await new Promise(r => setTimeout(r, dt || 16));
  }
  playing = false;
}




let coils=[];
function buildCoils(){
  coils.forEach(c=>c.el.remove()); coils=[];
  for(let i=0;i<N;i++){
    const el=document.createElement('div'); el.className='coil';
    const b=document.createElement('div'); b.className='body';
    const l=document.createElement('div'); l.className='capL';
    const r=document.createElement('div'); r.className='capR';
    el.append(b,l,r); arena.appendChild(el); coils.push({el});
  }
  placeCoils();
}
function placeCoils(){
  const r=trackRadius(); const {cx,cy}=ringCenter();
  for(let i=0;i<N;i++){
    const deg=wrap360(angles[i]);               
    const rad=(deg-90)*Math.PI/180;             
    const el=coils[i].el;
    el.style.left=(cx + r*Math.cos(rad))+"px";
    el.style.top =(cy + r*Math.sin(rad))+"px";
    el.style.setProperty('--rot', deg+'deg');
  }
}


function readUI(){
  N=clamp(parseInt(inpN.value||"4"),1,64);
  mode=parseInt(inpMode.value||"2"); dir=parseInt(inpDir.value||"1");
  rpm=parseFloat(inpRPM.value||"300"); stepMs=parseFloat(inpStep.value||"40");
  overlapDeg=parseFloat(inpOverlap.value||"20");
  phaseDeg  =parseFloat(inpPhase.value  ||"0");

  const arr=(inpAngles.value||"").split(",").map(s=>parseFloat(s.trim())).filter(v=>!Number.isNaN(v)).map(wrap360);
  if(arr.length===N) angles=arr; else { angles=[...Array(N)].map((_,i)=> i*(360/N)); inpAngles.value=angles.join(","); }

  if(coils.length!==N) buildCoils();
  placeCoils();
}


function activeByOverlap(pieceAbsAngle){
  const on=new Set();
  for(let i=0;i<N;i++){
    const diff = angDiff(pieceAbsAngle, angles[i]);
    if(diff <= overlapDeg/2){
      on.add(i);
      if(mode===2) on.add((i+1)%N);
    }
  }
  return [...on].sort((a,b)=>a-b);
}


function render(){
 
  discWrap.style.transform = `translate(-50%,-50%) rotate(${ang - 90}deg)`;

  
  const pieceAngleDeg = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--pieceAngle')) || 0;
  const pieceAbs = wrap360(ang + (pieceAngleDeg - 90) + phaseDeg);


  const act = activeByOverlap(pieceAbs);
  coils.forEach((c,i)=> c.el.classList.toggle('active', act.includes(i)));

  stats.textContent = `t=${t.toFixed(3)}s • piece_angel=${pieceAbs.toFixed(1)}° • rpm=${rpm.toFixed(1)}`;
}


function loop(now){
  if(!last) last=now;
  let dt=(now-last)/1000, step=stepMs/1000;
  while(dt>=step){ t+=step; ang=wrap360(ang + 360*(rpm/60)*step*dir); dt-=step; }
  last=now - dt*1000; render(); raf=requestAnimationFrame(loop);
}


btnStart.onclick=()=>{ readUI(); if(!raf){ last=0; raf=requestAnimationFrame(loop);} };
btnStop.onclick =()=>{ if(raf){ cancelAnimationFrame(raf); raf=null; } };
btnReset.onclick=()=>{ if(raf){ cancelAnimationFrame(raf); raf=null; } t=0; ang=0; render(); };

btn4x90.onclick = ()=>{ inpN.value=4;  inpAngles.value="0,90,180,270"; readUI(); render(); };
btn6x60.onclick = ()=>{ inpN.value=6;  inpAngles.value="0,60,120,180,240,300"; readUI(); render(); };
btn8x45.onclick = ()=>{ inpN.value=8;  inpAngles.value="0,45,90,135,180,225,270,315"; readUI(); render(); };
btn12x30.onclick= ()=>{ inpN.value=12; inpAngles.value=Array.from({length:12},(_,i)=>i*30).join(","); readUI(); render(); };

window.addEventListener('resize', ()=>{ placeCoils(); });


buildCoils(); readUI(); render();