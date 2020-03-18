//보스의 행동 및 패턴을 관리하는 스크립트입니다.
//_패턴 1: 높이 날아오르며 땅에 닿으면 폭발하는 바위를 여러 개 던진다.
//_패턴 2: 전방에 불을 뿜으며 공격한다.
//_패턴 3: 지각을 변동시켜 쥐어 잡는 손을 빠른 간격으로 소환한다.
//_기본패턴: 보스는 매 패턴 종료시 땅으로 파고들어 사라지며, 잠시 후 다음 패턴과 함께 솟아올라 공격한다.
//중복이 일어나지 않도록 다음 패턴은 항상 다른 것이 나오며, 패턴마다 진입시 시간 카운트 변수들이 존재합니다.
//패턴 시작시 isprepared 변수를 통해 패턴 진입 상태입을 판단합니다.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class patternmanage : MonoBehaviour {

    public Animator anim;
    public GameObject eventer1;
    public GameObject eventer2;
    public GameObject eventer3;
    private int pattern = 0;
    private bool patrunning1, patrunning2, patrunning3 = false;
    public float pat1cool = 1.0f;
    public float pat2cool = 1.0f;
    public float pat3cool = 1.0f;


    public float pat1time = 3.0f;
    public float pat2time = 3.0f;
    public float pat3time = 3.0f;

    public float prepcool = 1.0f;
    public float spdd = 10.0f;
    private bool fired,preparing,isprepared = false;
    private float patcooldown = 0.0f;
    private int prevpat = 8;
    private Vector3 playerpos;
    private Vector3 viewaxis;
    private bool isact = true;
    // Use this for initialization
    void Start () {
        eventer2.SetActive(false);
    }
	
	// Update is called once per frame
	void Update () {
        if (!isact)
        {
            transform.position = new Vector3(0, 0, 0);
            return;
        }
        if (preparing)
        {


            patcooldown += Time.deltaTime;

            if (patcooldown > 0.15f&& patcooldown < 2 && transform.position.y>-50)
            {
                transform.Translate(Vector3.down * spdd * Time.deltaTime, Space.Self);
            }
            if(patcooldown >1.5f)
            {
                anim.SetFloat("prep", 0.0f);
            }
            if (patcooldown > prepcool-0.1f)
            {
                if (patcooldown < prepcool)
                {
                    transform.position = new Vector3(playerpos.x, transform.position.y, transform.position.z);
                    playerpos = GameObject.Find("P1Root").transform.position;
                    viewaxis = new Vector3(transform.position.x - playerpos.x+ Random.Range(-0.1f,0.1f), 0, 0);
                    viewaxis = viewaxis.normalized;
                    transform.forward = viewaxis;
                    
                }

                transform.Translate(Vector3.up * spdd * Time.deltaTime, Space.Self);
            }
            if (patcooldown > prepcool)
            {
                if (transform.position.y > 0)
                {
                    transform.position = new Vector3(transform.position.x, 0f, transform.position.z);
                    preparing = false;
                    isprepared = true;
                    patcooldown = 0;
                    
                }
            }
        }
        if(!preparing && !patrunning1 && !patrunning2 && !patrunning3 && anim.GetCurrentAnimatorStateInfo(0).IsName("boss_golem_idle02"))
        {
            if (!isprepared)
            {
                preparing = true;
                anim.SetFloat("prep", 1.0f);
                prepcool = Random.Range(3.0f, 3.5f);
            }
            else
            {
                float patfloat = Random.Range(15.1f, 94.9f) / 30;

                pattern = (int)Mathf.Round(patfloat);

                if (pattern != prevpat)
                {
                    if (pattern == 1)
                        patrunning1 = true;
                    if (pattern == 2)
                        patrunning2 = true;
                    if (pattern == 3)
                        patrunning3 = true;
                    prevpat = pattern;

                }
            }
        }
        else
        {
            //Debug.Log(patrunning1);
            //Debug.Log(patrunning2);
            //Debug.Log(patrunning3);
            if (pattern == 1&&patrunning1)
            {
                anim.SetFloat("pat1", 1.0f);
                patcooldown += Time.deltaTime;

                if (!fired && patcooldown > pat1time)
                {
                    GameObject.Find("SpawnExplosion").GetComponent<spawnexplosion>().setOn();
                    fired = true;
                }

                if (patcooldown>pat1cool)
                {
                    patrunning1 = false;
                    anim.SetFloat("pat1",0.0f);
                    patcooldown = 0;
                    Debug.Log("1e");
                    fired = false;
                    isprepared = false;
                }
            }
            if (pattern == 2 && patrunning2)
            {
                anim.SetFloat("pat2", 1.0f);
                patcooldown += Time.deltaTime;
                if (!fired && patcooldown > pat2time)
                {
                    eventer2.SetActive(true);
                    fired = true;
                }
                if (patcooldown > pat2cool)
                {
                    patrunning2 = false;
                    anim.SetFloat("pat2", 0.0f);
                    patcooldown = 0;
                    Debug.Log("2e");
                    fired = false;
                    eventer2.SetActive(false);
                    isprepared = false;
                }
            }
            if (pattern == 3 && patrunning3)
            {
                anim.SetFloat("pat3", 1.0f);
                patcooldown += Time.deltaTime;
                if (!fired && patcooldown > pat3time)
                {
                    GameObject.Find("fallings").GetComponent<SpawnHands>().setOn();
                    fired = true;
                }
                if (patcooldown > pat3cool)
                {
                    patrunning3 = false;
                    patcooldown = 0;
                    anim.SetFloat("pat3", 0.0f);
                    Debug.Log("3e");
                    fired = false;
                    isprepared = false;
                }
            }
        }


	}
    public void Setact(bool k)
    {
        isact = k;
        //if (isact)
        //    gameObject.SetActive(true);
        //else
        //    gameObject.SetActive(false);
    }
}
