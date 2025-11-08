// old ascii dash animator
// - Jake Morgan

class pipe_animator {
  constructor(pSelect, options = {}){
    this.p = document.querySelector(pSelect);
    //this.intervalId = null;
    this.currentSize = {width: 0, height: 0};
    this.settings = {
      maxLength: 30, // since text keep it smaller
      minLength: 0, // 0 means no text
      interval: 100, //ms
      ...options
    };
    // could use phase to make them have different starting points
    this.phase = 1;
    this.text = "-";
    this.vel = 1;

    if (!this.p){
      throw new Error('no paragraph with selector "${pSelect}" found');
    }
  }
  start(){ // somewhere in here there is a undefined problem
    this.initialize();
    this.intervalId = setInterval(() => {
      this.animatePipe();
    }, this.settings.interval);
  }

  animatePipe(){
    const currentStr = this.p.textContent; 
    const currentLength = currentStr.length;
    if(this.vel == 1){
      this.text = currentStr;
      this.text += '-';
      this.p.textContent = this.text;
      if(currentLength > this.settings.maxLength){
        this.vel *= -1;
      }
    }
    // using the if logic is kinda wack here but it kinda works.
    if(this.vel == -1){
      this.text = currentStr;
      // should pop off the last pipe
      this.p.textContent = this.text.slice(0,-1);
      if(currentLength <= this.settings.minLength + 1){
        this.vel *= -1;
      }
    } 
  }
  initialize(){
    if(this.phase < 1){
      this.p.textContent = '-';
    }
    else{
      for(let i = 0; i<this.phase; i++){
        this.text += '-';
      } 
      this.p.textContent = this.text;
    }
  }
}

const MS_INTERVAL = 40;
const MAX_LENGTH = 22;

document.addEventListener('DOMContentLoaded',()=>{
  const animator_1 = new pipe_animator('#p0',{
    maxLength: MAX_LENGTH,
    minLength: 1,
    interval: MS_INTERVAL
  });
  const animator_2 = new pipe_animator('#p1',{
    maxLength: MAX_LENGTH,
    minLength: 1,
    interval: MS_INTERVAL 
  });
  const animator_3 = new pipe_animator('#p2',{
    maxLength: MAX_LENGTH,
    minLength: 1,
    interval: MS_INTERVAL
  });
  animator_1.phase = 1;
  animator_2.phase = 4;
  animator_3.phase = 8;
  animator_1.start();
  animator_2.start();
  animator_3.start();
});

