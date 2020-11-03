using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class AgentMove : MonoBehaviour
{
    public Transform home;
    NavMeshAgent agent;
    // Start is called before the first frame update
    /*void Start()
    {
        agent = this.GetComponent<NavMeshAgent>();
        agent.SetDestination(home.position);
    }*/
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.B))
        {
            agent = this.GetComponent<NavMeshAgent>();
            agent.SetDestination(home.position);
        }
    }
}
