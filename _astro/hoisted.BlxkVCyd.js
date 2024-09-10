const __vite__mapDeps=(i,m=__vite__mapDeps,d=(m.f||(m.f=["_astro/ui-core.CzBaGxGb.js","_astro/Tabs.astro_astro_type_script_index_0_lang.3nBd5krW.js"])))=>i.map(i=>d[i]);
import"./Tabs.astro_astro_type_script_index_0_lang.3nBd5krW.js";const E=document.getElementById("starlight__sidebar"),v=E?.querySelector("sl-sidebar-state-persist"),S="sl-sidebar-state",w=()=>{let s=[];const e=v?.dataset.hash||"";try{const n=sessionStorage.getItem(S),t=JSON.parse(n||"{}");Array.isArray(t.open)&&t.hash===e&&(s=t.open)}catch{}return{hash:e,open:s,scroll:E?.scrollTop||0}},b=s=>{try{sessionStorage.setItem(S,JSON.stringify(s))}catch{}},L=()=>b(w()),q=(s,e)=>{const n=w();n.open[e]=s,b(n)};v?.addEventListener("click",s=>{if(!(s.target instanceof Element))return;const e=s.target.closest("summary")?.closest("details");if(!e)return;const n=e.querySelector("sl-sidebar-restore"),t=parseInt(n?.dataset.index||"");isNaN(t)||q(!e.open,t)});addEventListener("visibilitychange",()=>{document.visibilityState==="hidden"&&L()});addEventListener("pageHide",L);class C extends HTMLElement{constructor(){super();const e=this.querySelector("select");e&&e.addEventListener("change",n=>{n.currentTarget instanceof HTMLSelectElement&&(window.location.pathname=n.currentTarget.value)})}}customElements.define("starlight-lang-select",C);const H="modulepreload",A=function(s){return"/RIOT/"+s},y={},R=function(e,n,t){let i=Promise.resolve();if(n&&n.length>0){document.getElementsByTagName("link");const r=document.querySelector("meta[property=csp-nonce]"),o=r?.nonce||r?.getAttribute("nonce");i=Promise.all(n.map(c=>{if(c=A(c),c in y)return;y[c]=!0;const h=c.endsWith(".css"),a=h?'[rel="stylesheet"]':"";if(document.querySelector(`link[href="${c}"]${a}`))return;const d=document.createElement("link");if(d.rel=h?"stylesheet":H,h||(d.as="script"),d.crossOrigin="",d.href=c,o&&d.setAttribute("nonce",o),document.head.appendChild(d),h)return new Promise((u,l)=>{d.addEventListener("load",u),d.addEventListener("error",()=>l(new Error(`Unable to preload CSS for ${c}`)))})}))}return i.then(()=>e()).catch(r=>{const o=new Event("vite:preloadError",{cancelable:!0});if(o.payload=r,window.dispatchEvent(o),!o.defaultPrevented)throw r})};class _ extends HTMLElement{constructor(){super();const e=this.querySelector("button[data-open-modal]"),n=this.querySelector("button[data-close-modal]"),t=this.querySelector("dialog"),i=this.querySelector(".dialog-frame"),r=l=>{("href"in(l.target||{})||document.body.contains(l.target)&&!i.contains(l.target))&&c()},o=l=>{t.showModal(),document.body.toggleAttribute("data-search-modal-open",!0),this.querySelector("input")?.focus(),l?.stopPropagation(),window.addEventListener("click",r)},c=()=>t.close();e.addEventListener("click",o),e.disabled=!1,n.addEventListener("click",c),t.addEventListener("close",()=>{document.body.toggleAttribute("data-search-modal-open",!1),window.removeEventListener("click",r)}),window.addEventListener("keydown",l=>{(l.metaKey===!0||l.ctrlKey===!0)&&l.key==="k"&&(t.open?c():o(),l.preventDefault())});let h={};try{h=JSON.parse(this.dataset.translations||"{}")}catch{}const u=this.dataset.stripTrailingSlash!==void 0?l=>l.replace(/(.)\/(#.*)?$/,"$1$2"):l=>l;window.addEventListener("DOMContentLoaded",()=>{(window.requestIdleCallback||(g=>setTimeout(g,1)))(async()=>{const{PagefindUI:g}=await R(async()=>{const{PagefindUI:m}=await import("./ui-core.CzBaGxGb.js");return{PagefindUI:m}},__vite__mapDeps([0,1]));new g({element:"#starlight__search",baseUrl:"/RIOT/",bundlePath:"/RIOT/".replace(/\/$/,"")+"/pagefind/",showImages:!1,translations:h,showSubResults:!0,processResult:m=>{m.url=u(m.url),m.sub_results=m.sub_results.map(p=>(p.url=u(p.url),p))}})})})}}customElements.define("site-search",_);const T="starlight-theme",I=s=>s==="auto"||s==="dark"||s==="light"?s:"auto",M=()=>I(typeof localStorage<"u"&&localStorage.getItem(T));function O(s){typeof localStorage<"u"&&localStorage.setItem(T,s==="light"||s==="dark"?s:"")}const z=()=>matchMedia("(prefers-color-scheme: light)").matches?"light":"dark";function f(s){StarlightThemeProvider.updatePickers(s),document.documentElement.dataset.theme=s==="auto"?z():s,O(s)}matchMedia("(prefers-color-scheme: light)").addEventListener("change",()=>{M()==="auto"&&f("auto")});class P extends HTMLElement{constructor(){super(),f(M()),this.querySelector("select")?.addEventListener("change",e=>{e.currentTarget instanceof HTMLSelectElement&&f(I(e.currentTarget.value))})}}customElements.define("starlight-theme-select",P);const $="starlight-image-zoom-zoomable";customElements.define("starlight-image-zoom",class x extends HTMLElement{#e;#o=[];#s;#i=this.querySelector("template");#t={image:"starlight-image-zoom-image",opened:"starlight-image-zoom-opened",source:"starlight-image-zoom-source"};static#u=!1;constructor(){super();const e=()=>{(window.requestIdleCallback??(t=>setTimeout(t,1)))(()=>{const t=[...document.querySelectorAll($)];t.length!==0&&(this.#h(t),document.addEventListener("click",this.#r),window.addEventListener("resize",this.#a))})};window.addEventListener("DOMContentLoaded",e,{once:!0}),document.addEventListener("astro:after-preparation",()=>{document.removeEventListener("click",this.#r),window.removeEventListener("resize",this.#a)},{once:!0}),x.#u||=document.addEventListener("astro:after-swap",e)===void 0}#h(e){for(const n of e){const t=n.querySelector("img");t&&(this.#o.push(t),n.querySelector("button")?.addEventListener("click",i=>{i.stopPropagation(),this.#l(t)}))}}#r=({target:e})=>{if(!(e instanceof HTMLElement&&e.closest("figcaption"))){if(this.#e){this.#n();return}e instanceof HTMLImageElement&&this.#o.includes(e)&&this.#l(e)}};#a=()=>{this.#n(!0)};#c=()=>{this.#n()};#m=e=>{e.preventDefault(),this.#n()};#l(e){if(!this.#i||this.#e)return;this.#s=document.activeElement;const n=document.createElement("div");n.append(this.#i.content.cloneNode(!0));const t=n.querySelector("dialog"),i=t?.querySelector("figure");if(!t||!i)return;const r={overflow:document.body.style.overflow,width:document.body.style.width},o=document.body.clientWidth;document.body.style.overflow="hidden",document.body.style.width=`${o}px`,document.querySelector("header")?.style.setProperty("padding-inline-end",`calc(var(--sl-nav-pad-x) + ${window.innerWidth-o}px)`);const c=this.#p(e);e.classList.add(this.#t.source),c.classList.add(this.#t.image),i.append(c),this.#g(e.getAttribute("alt"),i),document.body.append(n),document.addEventListener("wheel",this.#c,{once:!0}),t.addEventListener("cancel",this.#m),t.showModal(),c.style.transform=this.#f(e,i),document.body.classList.add(this.#t.opened),this.#e={body:r,dialog:t,image:e,zoomedImage:c}}#n(e=!1){if(window.removeEventListener("wheel",this.#c),!this.#e)return;const{zoomedImage:n}=this.#e;n.style.transform="",document.body.classList.remove(this.#t.opened);const{matches:t}=window.matchMedia("(prefers-reduced-motion: reduce)");e||t?this.#d():n.addEventListener("transitionend",this.#d,{once:!0})}#d=()=>{if(!this.#e)return;const{dialog:e,image:n}=this.#e;n.classList.remove(this.#t.source),e.parentElement?.remove(),document.body.style.overflow=this.#e.body.overflow,document.body.style.width=this.#e.body.width,document.querySelector("header")?.style.setProperty("padding-inline-end","var(--sl-nav-pad-x)"),this.#e=void 0,this.#s instanceof HTMLElement&&this.#s.focus()};#g(e,n){if(Object.hasOwn(this.dataset,"hideCaption")||(e=e?.trim()??"",e.length===0))return;const t=document.createElement("figcaption");t.ariaHidden="true",t.textContent=e,n.append(t)}#p(e){const{height:n,left:t,top:i,width:r}=e.getBoundingClientRect(),o=e.cloneNode(!0);return o.removeAttribute("id"),o.style.position="absolute",o.style.width=`${r}px`,o.style.height=`${n}px`,o.style.top=`${i}px`,o.style.left=`${t}px`,o.style.transform="",e.parentElement?.tagName==="PICTURE"&&e.currentSrc&&(o.src=e.currentSrc),o}#f(e,n){const t=this.#y(e),i=e.getBoundingClientRect(),r=n.getBoundingClientRect(),o=t?r.height:e.naturalHeight,c=t?r.width:e.naturalWidth,h=Math.min(Math.max(i.width,c),r.width)/i.width,a=Math.min(Math.max(i.height,o),r.height)/i.height,d=Math.min(h,a),u=(-i.left+(r.width-i.width)/2+r.left)/d,l=(-i.top+(r.height-i.height)/2+r.top)/d;return`scale(${d}) translate3d(${u}px, ${l}px, 0)`}#y(e){return e.currentSrc.toLowerCase().endsWith(".svg")}});class N extends HTMLElement{constructor(){super(),this.btn=this.querySelector("button"),this.btn.addEventListener("click",()=>this.toggleExpanded());const e=this.closest("nav");e&&e.addEventListener("keyup",n=>this.closeOnEscape(n))}setExpanded(e){this.setAttribute("aria-expanded",String(e)),document.body.toggleAttribute("data-mobile-menu-expanded",e)}toggleExpanded(){this.setExpanded(this.getAttribute("aria-expanded")!=="true")}closeOnEscape(e){e.code==="Escape"&&(this.setExpanded(!1),this.btn.focus())}}customElements.define("starlight-menu-button",N);const B="_top";class k extends HTMLElement{constructor(){super(),this._current=this.querySelector('a[aria-current="true"]'),this.minH=parseInt(this.dataset.minH||"2",10),this.maxH=parseInt(this.dataset.maxH||"3",10),this.onIdle=e=>(window.requestIdleCallback||(n=>setTimeout(n,1)))(e),this.init=()=>{const e=[...this.querySelectorAll("a")],n=a=>{if(a instanceof HTMLHeadingElement){if(a.id===B)return!0;const d=a.tagName[1];if(d){const u=parseInt(d,10);if(u>=this.minH&&u<=this.maxH)return!0}}return!1},t=a=>{if(!a)return null;const d=a;for(;a;){if(n(a))return a;for(a=a.previousElementSibling;a?.lastElementChild;)a=a.lastElementChild;const u=t(a);if(u)return u}return t(d.parentElement)},i=a=>{for(const{isIntersecting:d,target:u}of a){if(!d)continue;const l=t(u);if(!l)continue;const g=e.find(m=>m.hash==="#"+encodeURIComponent(l.id));if(g){this.current=g;break}}},r=document.querySelectorAll("main [id], main [id] ~ *, main .content > *");let o;const c=()=>{o||(o=new IntersectionObserver(i,{rootMargin:this.getRootMargin()}),r.forEach(a=>o.observe(a)))};c();let h;window.addEventListener("resize",()=>{o&&o.disconnect(),clearTimeout(h),h=setTimeout(()=>this.onIdle(c),200)})},this.onIdle(()=>this.init())}set current(e){e!==this._current&&(this._current&&this._current.removeAttribute("aria-current"),e.setAttribute("aria-current","true"),this._current=e)}getRootMargin(){const e=document.querySelector("header")?.getBoundingClientRect().height||0,n=this.querySelector("summary")?.getBoundingClientRect().height||0,t=e+n+32,i=t+53,r=document.documentElement.clientHeight;return`-${t}px 0% ${i-r}px`}}customElements.define("starlight-toc",k);class U extends k{set current(e){super.current=e;const n=this.querySelector(".display-current");n&&(n.textContent=e.textContent)}constructor(){super();const e=this.querySelector("details");if(!e)return;const n=()=>{e.open=!1};e.querySelectorAll("a").forEach(t=>{t.addEventListener("click",n)}),window.addEventListener("click",t=>{e.contains(t.target)||n()}),window.addEventListener("keydown",t=>{if(t.key==="Escape"&&e.open){const i=e.contains(document.activeElement);if(n(),i){const r=e.querySelector("summary");r&&r.focus()}}})}}customElements.define("mobile-starlight-toc",U);export{R as _};
